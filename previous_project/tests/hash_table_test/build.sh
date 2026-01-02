#!/bin/bash

PROJECT_ROOT="/home/ammar-anas/Documents/ITU/BSAI24056-Semester-03/DSA/Alacrity Chess/alacrity-chess"

mkdir -p "$PROJECT_ROOT/bin"
mkdir -p "$PROJECT_ROOT/tests/hash_table_test/test_data"

g++ -std=c++17 -pthread \
    "$PROJECT_ROOT/src/server/core/hash_table/hash_table.cpp" \
    "$PROJECT_ROOT/src/server/utils/logger/logger.cpp" \
    "$PROJECT_ROOT/tests/hash_table_test/main.cpp" \
    -I"$PROJECT_ROOT/includes" \
    -o "$PROJECT_ROOT/bin/hash_table_test"

if [ -f "$PROJECT_ROOT/bin/hash_table_test" ]; then
    echo "Hash table test built successfully"
else
    echo "Build failed"
    exit 1
fi
