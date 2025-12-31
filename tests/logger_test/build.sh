#!/bin/bash

PROJECT_ROOT="/home/ammar-anas/Documents/ITU/BSAI24056-Semester-03/DSA/Alacrity Chess/alacrity-chess"

mkdir -p "$PROJECT_ROOT/bin"

g++ -std=c++17 -pthread \
    "$PROJECT_ROOT/src/server/utils/logger/logger.cpp" \
    "$PROJECT_ROOT/tests/logger_test/main.cpp" \
    -I"$PROJECT_ROOT/includes" \
    -o "$PROJECT_ROOT/bin/logger_test"

if [ -f "$PROJECT_ROOT/bin/logger_test" ]; then
    echo "Logger test built successfully"
else
    echo "Build failed"
    exit 1
fi
