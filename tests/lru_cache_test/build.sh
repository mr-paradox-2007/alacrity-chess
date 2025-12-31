#!/bin/bash
cd "$(dirname "$0")" || exit 1
g++ -std=c++17 -pthread -I../../includes main.cpp -o ../../bin/lru_cache_test 2>&1
