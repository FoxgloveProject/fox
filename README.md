# fox
The package manager for the Foxglove Linux distribution, written in C++.

## Overview

`fox` is a simple, modern package manager designed for performance, clarity, and ease of use. It handles fetching, installing, updating, and removing software packages on Foxglove Linux.

## The `.fox` Package Format

A `.fox` package is a `tar.xz` compressed archive containing:
1.  **Package Data**: The actual files to be installed, maintaining their directory structure relative to the system root (`/`).
2.  **Metadata File**: A `fox.json` file in the root of the archive with information about the package.

### `fox.json` Example

```json
{
  "name": "example-package",
  "version": "1.2.3",
  "description": "An example package for demonstration.",
  "arch": "x86_64",
  "license": "GPL-3.0-or-later",
  "dependencies": [
    "glibc>=2.31",
    "another-package==1.0.0"
  ],
  "maintainer": "Foxglove Maintainers <contact@foxglove.org>"
}
```

## Installation

### Prerequisites

To build `fox` from source, you need:
- CMake (version 3.16 or higher)
- A C++ compiler with C++17 support (GCC, Clang, or MSVC)
- Git

### Building from Source

1. Clone the repository:
```bash
git clone https://github.com/FoxgloveProject/fox.git
cd fox
```

2. Create a build directory and compile:
```bash
mkdir build
cd build
cmake ..
make
```

3. (Optional) Install to system:
```bash
sudo make install
```

The executable will be available as `fox` in the build directory (or system-wide if installed).

## Usage

`fox` provides a simple command-line interface with the following commands:

### Basic Commands

*   **Install packages**: `fox install <package1> [package2] ...`
*   **Remove packages**: `fox remove <package1> [package2] ...`
*   **Search packages**: `fox search <query>`

### Examples

```bash
# Install a single package
fox install firefox

# Install multiple packages
fox install firefox vim git

# Remove packages
fox remove old-package

# Search for packages
fox search editor

# Show help
fox --help

# Show version
fox --version
```

**Note**: Package installation and removal typically require root privileges (`sudo`).

## Development

### Project Structure

```
fox/
├── src/           # Source code
│   └── main.cpp   # Main application entry point
├── CMakeLists.txt # CMake build configuration
├── README.md      # This file
└── build/         # Build directory (created during build)
```

### Dependencies

- **CLI11**: Command-line argument parsing (automatically downloaded via CMake)
- **C++17**: Modern C++ features

### Building for Development

```bash
# Debug build
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Release build
mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
