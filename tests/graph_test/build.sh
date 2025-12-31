#!/bin/bash
cd "$(dirname "$0")" || exit 1
g++ -std=c++17 -pthread -I../../includes ../../src/server/utils/logger/logger.cpp ../../src/server/core/graph/graph.cpp main.cpp -o ../../bin/graph_test 2>&1
