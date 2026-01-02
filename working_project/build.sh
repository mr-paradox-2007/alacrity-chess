#!/bin/bash

cd "$(dirname "$0")"

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc)

# Copy binary to bin directory
mkdir -p ../bin
cp chess_client ../bin/
if [ -f chess_server ]; then
    cp chess_server ../bin/
fi

echo "Build complete! Binary in bin/ directory"
