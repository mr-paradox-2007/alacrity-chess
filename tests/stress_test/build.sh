#!/bin/bash

mkdir -p ../../bin
cd /home/ammar-anas/Documents/ITU/BSAI24056-Semester-03/DSA/Alacrity\ Chess/alacrity-chess/tests/stress_test
g++ -std=c++17 -pthread ../../src/server/utils/logger/logger.cpp main.cpp -I../../includes -o ../../bin/stress_test

if [ -f ../../bin/stress_test ]; then
    echo "Stress test built successfully"
else
    echo "Build failed"
    exit 1
fi
