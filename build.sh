#!/bin/bash

set -e

echo "=========================================="
echo "Alacrity Chess Server Build Script"
echo "=========================================="

if [ ! -d "bin" ]; then
    echo "[BUILD] Creating bin directory..."
    mkdir -p bin
fi

if [ ! -d "data" ]; then
    echo "[BUILD] Creating data directory..."
    mkdir -p data
fi

if [ ! -d "logs" ]; then
    echo "[BUILD] Creating logs directory..."
    mkdir -p logs
fi

echo "[BUILD] Compiling server components..."
echo "[BUILD] Command:"
echo "g++ -std=c++17 -pthread \\"
echo "    -I./includes \\"
echo "    -I/usr/include \\"
echo "    src/server/core/hash_table/hash_table.cpp \\"
echo "    src/server/core/b_tree/b_tree.cpp \\"
echo "    src/server/core/graph/graph.cpp \\"
echo "    src/server/core/heap/heap.cpp \\"
echo "    src/server/utils/logger/logger.cpp \\"
echo "    src/server/network/server.cpp \\"
echo "    src/server/api/auth_api.cpp \\"
echo "    src/server/main.cpp \\"
echo "    -o bin/alacrity_chess_server"
echo ""

g++ -std=c++17 -pthread \
    -I./includes \
    -I/usr/include \
    src/server/core/hash_table/hash_table.cpp \
    src/server/core/b_tree/b_tree.cpp \
    src/server/core/graph/graph.cpp \
    src/server/core/heap/heap.cpp \
    src/server/utils/logger/logger.cpp \
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
    echo "Binary size: $(du -h bin/alacrity_chess_server | cut -f1)"
    echo ""
    echo "To run the server:"
    echo "  ./bin/alacrity_chess_server [port]"
    echo ""
    echo "To test authentication:"
    echo "  chmod +x test_auth.sh"
    echo "  ./test_auth.sh"
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