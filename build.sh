#!/bin/bash

# ToneTrigger Build Script
# This script builds the ToneTrigger application using CMake

set -e  # Exit on any error

echo "=== ToneTrigger Build Script ==="

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Please run this script from the project root directory."
    exit 1
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building ToneTrigger..."
cmake --build . --config Release

echo "=== Build completed successfully! ==="
echo "Executable location: build/bin/ToneTrigger"

# On macOS, also create a .app bundle
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Creating macOS app bundle..."
    mkdir -p ToneTrigger.app/Contents/MacOS
    cp bin/ToneTrigger ToneTrigger.app/Contents/MacOS/
    
    # Create Info.plist
    cat > ToneTrigger.app/Contents/Info.plist << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>ToneTrigger</string>
    <key>CFBundleIdentifier</key>
    <string>com.toneTrigger.app</string>
    <key>CFBundleName</key>
    <string>ToneTrigger</string>
    <key>CFBundleVersion</key>
    <string>1.0.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0.0</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleSignature</key>
    <string>????</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.13</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
EOF
    
    echo "macOS app bundle created: build/ToneTrigger.app"
fi

echo ""
echo "=== Build Summary ==="
echo "Platform: $(uname -s)"
echo "Architecture: $(uname -m)"
echo "Build type: Release"
echo ""
echo "To run the application:"
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "  ./build/ToneTrigger.app/Contents/MacOS/ToneTrigger"
    echo "  or double-click: ./build/ToneTrigger.app"
else
    echo "  ./build/bin/ToneTrigger"
fi 