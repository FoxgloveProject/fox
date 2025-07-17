cmake_minimum_required(VERSION 3.16)
project(fox-pm VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add a target for the main executable
add_executable(fox src/main.cpp)
target_include_directories(fox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# --- Dependencies ---
# We will use FetchContent to get CLI11 for argument parsing.
# This avoids needing the user to install it system-wide.
include(FetchContent)

FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.4.1
)

FetchContent_MakeAvailable(CLI11)

# Link our executable against the CLI11 library
target_link_libraries(fox PRIVATE CLI11::CLI11)

# --- Installation ---
# This allows `cmake --install` to place the binary in a system location
install(TARGETS fox DESTINATION bin)