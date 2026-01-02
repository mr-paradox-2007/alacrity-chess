#!/bin/bash

echo "Testing Chess Server..."

# Start server in background
./bin/chess_server &
SERVER_PID=$!

# Wait for server to start
sleep 2

# Test health endpoint
echo "Testing /health endpoint..."
curl -s http://localhost:8080/health
echo ""

# Test static file serving
echo "Testing static file serving (index.html)..."
curl -s -o /dev/null -w "HTTP Status: %{http_code}\n" http://localhost:8080/

# Test CSS file
echo "Testing CSS file..."
curl -s -o /dev/null -w "HTTP Status: %{http_code}\n" http://localhost:8080/css/main.css

# Test JS file
echo "Testing JS file..."
curl -s -o /dev/null -w "HTTP Status: %{http_code}\n" http://localhost:8080/js/api.js

# Test registration
echo "Testing registration..."
curl -s -X POST http://localhost:8080/auth/register \
  -H "Content-Type: application/json" \
  -d '{"username":"testuser","password":"testpass"}'
echo ""

# Cleanup
kill $SERVER_PID 2>/dev/null || true
echo "Test completed!"

