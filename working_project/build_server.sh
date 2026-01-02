#!/bin/bash

set -e

echo "Building Chess Platform Server..."

cd "$(dirname "$0")"

mkdir -p bin
mkdir -p server/bin

g++ -std=c++17 -pthread -o bin/chess_server \
    server/main.cpp \
    -I. \
    -lm

cp bin/chess_server server/bin/chess_server

echo "Build completed successfully!"
echo "Server built in: bin/chess_server"
echo "Server built in: server/bin/chess_server"
echo ""
echo "To run from working_project: ./bin/chess_server"
echo "To run from server/bin: ./server/bin/chess_server"

