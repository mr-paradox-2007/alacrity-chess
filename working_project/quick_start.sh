#!/bin/bash

echo "Chess Platform - Quick Start"
echo "============================"
echo ""

cd /home/ammar-anas/Documents/ITU/BSAI24056-Semester-03/DSA/Alacrity\ Chess/alacrity-chess/working_project

echo "1. Testing data structures..."
./tests/ds_test

echo ""
echo "2. Building server..."
./build.sh

echo ""
echo "3. Server is ready!"
echo "   Run: ./bin/chess_server"
echo ""
echo "4. Open client:"
echo "   file://$(pwd)/client/index.html"
echo ""
echo "Features:"
echo "  - User registration & authentication"
echo "  - Real-time friend system with recommendations"
echo "  - Match history with B-Tree queries"
echo "  - Leaderboard with max heap"
echo "  - Session management with LRU cache"
echo ""
echo "Data Structures:"
echo "  - Hash Table (authentication, sessions)"
echo "  - B-Tree (match history, range queries)"
echo "  - Graph (friend system, BFS recommendations)"
echo "  - Max Heap (leaderboard, matchmaking)"
echo "  - LRU Cache (session caching)"
