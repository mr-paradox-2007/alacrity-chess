#!/bin/bash

set -e

echo "Building Chess Platform Server..."

mkdir -p bin

g++ -std=c++17 -pthread -o bin/chess_server \
    server/main.cpp \
    -I. \
    -lm

echo "Build completed successfully!"
echo "To run: ./bin/chess_server"
