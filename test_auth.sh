#!/bin/bash

# Test script for Alacrity Chess authentication API

PORT=8888
SERVER_BIN="./bin/alacrity_chess_server"

echo "═══════════════════════════════════════════════════════════"
echo "  Alacrity Chess - Authentication API Test"
echo "═══════════════════════════════════════════════════════════"
echo ""

# Start server in background
echo "[1/4] Starting server on port $PORT..."
$SERVER_BIN $PORT > /tmp/server.log 2>&1 &
SERVER_PID=$!
sleep 2

# Check if server is running
if ! ps -p $SERVER_PID > /dev/null; then
    echo "✗ Server failed to start"
    cat /tmp/server.log
    exit 1
fi
echo "✓ Server started (PID: $SERVER_PID)"
echo ""

# Test 1: Register new user
echo "[2/4] Testing REGISTER..."
REGISTER_JSON='{"action":"register","username":"alice","password":"securepass123"}'
REGISTER_RESPONSE=$(echo -n "$REGISTER_JSON" | nc localhost $PORT 2>/dev/null)
echo "  Request: $REGISTER_JSON"
echo "  Response: $REGISTER_RESPONSE"

if echo "$REGISTER_RESPONSE" | grep -q '"status":"success"'; then
    echo "  ✓ Registration successful"
else
    echo "  ✗ Registration failed"
fi
echo ""

# Test 2: Login with correct password
echo "[3/4] Testing LOGIN (correct password)..."
LOGIN_JSON='{"action":"login","username":"alice","password":"securepass123"}'
LOGIN_RESPONSE=$(echo -n "$LOGIN_JSON" | nc localhost $PORT 2>/dev/null)
echo "  Request: $LOGIN_JSON"
echo "  Response: $LOGIN_RESPONSE"

if echo "$LOGIN_RESPONSE" | grep -q '"status":"success"'; then
    echo "  ✓ Login successful"
else
    echo "  ✗ Login failed"
fi
echo ""

# Test 3: Login with wrong password
echo "[4/4] Testing LOGIN (wrong password)..."
WRONG_LOGIN_JSON='{"action":"login","username":"alice","password":"wrongpass"}'
WRONG_LOGIN_RESPONSE=$(echo -n "$WRONG_LOGIN_JSON" | nc localhost $PORT 2>/dev/null)
echo "  Request: $WRONG_LOGIN_JSON"
echo "  Response: $WRONG_LOGIN_RESPONSE"

if echo "$WRONG_LOGIN_RESPONSE" | grep -q '"status":"error"'; then
    echo "  ✓ Correctly rejected wrong password"
else
    echo "  ✗ Should have rejected wrong password"
fi
echo ""

# Shutdown server
echo "Shutting down server..."
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  Test Complete!"
echo "═══════════════════════════════════════════════════════════"
