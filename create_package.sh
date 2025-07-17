#!/bin/bash

# Fox Package Creator
# This script creates a .fox package from a directory

PACKAGE_NAME=${1:-"example"}
PACKAGE_VERSION=${2:-"1.0.0"}
PACKAGE_DIR=${3:-"package-files"}

echo "Creating .fox package: $PACKAGE_NAME-$PACKAGE_VERSION"

# Create package directory structure
mkdir -p "$PACKAGE_DIR"/{bin,lib,share/doc,etc}

# Create a sample executable
cat > "$PACKAGE_DIR/bin/$PACKAGE_NAME" << 'EOF'
#!/bin/bash
echo "Hello from $PACKAGE_NAME!"
echo "This is a sample package created with fox-pm"
EOF
chmod +x "$PACKAGE_DIR/bin/$PACKAGE_NAME"

# Create a sample library
cat > "$PACKAGE_DIR/lib/lib$PACKAGE_NAME.so" << 'EOF'
# This is a dummy shared library file
# In a real package, this would contain actual compiled code
EOF

# Create documentation
cat > "$PACKAGE_DIR/share/doc/README" << EOF
$PACKAGE_NAME Package
Version: $PACKAGE_VERSION

This is a sample package created for testing the fox package manager.
EOF

# Create configuration file
cat > "$PACKAGE_DIR/etc/$PACKAGE_NAME.conf" << EOF
# Configuration file for $PACKAGE_NAME
# Version: $PACKAGE_VERSION

# Sample configuration
setting1 = value1
setting2 = value2
EOF

# Create the fox.json metadata file
cat > "$PACKAGE_DIR/fox.json" << EOF
{
  "name": "$PACKAGE_NAME",
  "version": "$PACKAGE_VERSION",
  "description": "A sample package for testing fox package manager",
  "arch": "x86_64",
  "license": "MIT",
  "dependencies": [],
  "maintainer": "Foxglove Maintainers <contact@foxglove.org>",
  "files": [
    "bin/$PACKAGE_NAME",
    "lib/lib$PACKAGE_NAME.so",
    "share/doc/README",
    "etc/$PACKAGE_NAME.conf"
  ]
}
EOF

# Create the .fox package (tar.xz archive)
echo "Creating $PACKAGE_NAME.fox..."
cd "$PACKAGE_DIR"
tar -cJf "../$PACKAGE_NAME.fox" *
cd ..

echo "Package created: $PACKAGE_NAME.fox"
echo "Package contents:"
tar -tJf "$PACKAGE_NAME.fox"

echo ""
echo "To install this package with fox:"
echo "  ./fox install $PACKAGE_NAME"
echo ""
echo "Note: You'll need to host this .fox file at a URL that your fox package manager can access." 