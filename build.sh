#!/bin/bash

# Build script for Alacrity Chess Server - Phase 1
# This compiles the server with all auth API components

set -e

echo "=========================================="
echo "Alacrity Chess Server Build Script"
echo "=========================================="

# Check if bin directory exists
if [ ! -d "bin" ]; then
    echo "[BUILD] Creating bin directory..."
    mkdir -p bin
fi

echo "[BUILD] Compiling server components..."
echo "[BUILD] Command:"
echo "g++ -std=c++17 -pthread \\"
echo "    -I./includes \\"
echo "    -I/usr/include \\"
echo "    src/server/network/server.cpp \\"
echo "    src/server/api/auth_api.cpp \\"
echo "    src/server/main.cpp \\"
echo "    -o bin/alacrity_chess_server"
echo ""

g++ -std=c++17 -pthread \
    -I./includes \
    -I/usr/include \
    src/server/network/server.cpp \
    src/server/api/auth_api.cpp \
    src/server/main.cpp \
    -o bin/alacrity_chess_server \
    2>&1

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "✓ Server compiled successfully"
    echo "=========================================="
    echo ""
    echo "Binary location: bin/alacrity_chess_server"
    echo ""
    echo "To run the server:"
    echo "  ./bin/alacrity_chess_server"
    echo ""
    exit 0
else
    echo ""
    echo "=========================================="
    echo "✗ Compilation failed"
    echo "=========================================="
    echo ""
    exit 1
fi
