#!/bin/bash
cd "$(dirname "$0")" || exit 1
g++ -std=c++17 -pthread -I../../includes ../../src/server/core/heap/heap.cpp main.cpp -o ../../bin/heap_test 2>&1
