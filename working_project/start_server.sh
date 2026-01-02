#!/bin/bash

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Kill any existing server
pkill -f chess_server 2>/dev/null || true
sleep 1

# Clear port 8080
lsof -ti:8080 | xargs kill -9 2>/dev/null || true
sleep 1

# Build the server
echo "Building server..."
./build.sh

# Start the server
echo ""
echo "Starting Chess Platform Server on port 8080..."
echo "Server running from: $(pwd)"
echo "Open http://localhost:8080 in your browser"
echo "Press Ctrl+C to stop the server"
echo ""

./bin/chess_server

