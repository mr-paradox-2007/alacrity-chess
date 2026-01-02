#!/bin/bash

PORT=8080
SERVER_BIN="./bin/alacrity_chess_server"

echo "═══════════════════════════════════════════════════════════"
echo "  Alacrity Chess - Authentication API Test"
echo "═══════════════════════════════════════════════════════════"
echo ""

if [ ! -f "$SERVER_BIN" ]; then
    echo "Error: Server binary not found. Run ./build.sh first"
    exit 1
fi

echo "[1/5] Starting server on port $PORT..."
$SERVER_BIN $PORT > server.log 2>&1 &
SERVER_PID=$!
sleep 2

if ! ps -p $SERVER_PID > /dev/null; then
    echo "✗ Server failed to start"
    cat server.log
    exit 1
fi
echo "✓ Server started (PID: $SERVER_PID)"
echo ""

echo "[2/5] Testing REGISTER..."
REGISTER_JSON='{"action":"register","username":"alice","password":"securepass123"}'
echo "  Request: $REGISTER_JSON"
REGISTER_RESPONSE=$(echo -n "$REGISTER_JSON" | nc -w 1 localhost $PORT 2>/dev/null)
echo "  Response: $REGISTER_RESPONSE"

if echo "$REGISTER_RESPONSE" | grep -q '"status":"success"'; then
    echo "  ✓ Registration successful"
else
    echo "  ✗ Registration failed"
fi
echo ""

sleep 1

echo "[3/5] Testing LOGIN (correct password)..."
LOGIN_JSON='{"action":"login","username":"alice","password":"securepass123"}'
echo "  Request: $LOGIN_JSON"
LOGIN_RESPONSE=$(echo -n "$LOGIN_JSON" | nc -w 1 localhost $PORT 2>/dev/null)
echo "  Response: $LOGIN_RESPONSE"

if echo "$LOGIN_RESPONSE" | grep -q '"status":"success"'; then
    echo "  ✓ Login successful"
else
    echo "  ✗ Login failed"
fi
echo ""

sleep 1

echo "[4/5] Testing LOGIN (wrong password)..."
WRONG_LOGIN_JSON='{"action":"login","username":"alice","password":"wrongpass"}'
echo "  Request: $WRONG_LOGIN_JSON"
WRONG_LOGIN_RESPONSE=$(echo -n "$WRONG_LOGIN_JSON" | nc -w 1 localhost $PORT 2>/dev/null)
echo "  Response: $WRONG_LOGIN_RESPONSE"

if echo "$WRONG_LOGIN_RESPONSE" | grep -q '"status":"error"'; then
    echo "  ✓ Correctly rejected wrong password"
else
    echo "  ✗ Should have rejected wrong password"
fi
echo ""

sleep 1

echo "[5/5] Testing persistence..."
echo "  Stopping server..."
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
sleep 1

echo "  Restarting server..."
$SERVER_BIN $PORT > server.log 2>&1 &
SERVER_PID=$!
sleep 2

if ! ps -p $SERVER_PID > /dev/null; then
    echo "  ✗ Server failed to restart"
    exit 1
fi

echo "  Testing login with persisted user..."
PERSIST_LOGIN_JSON='{"action":"login","username":"alice","password":"securepass123"}'
PERSIST_RESPONSE=$(echo -n "$PERSIST_LOGIN_JSON" | nc -w 1 localhost $PORT 2>/dev/null)
echo "  Response: $PERSIST_RESPONSE"

if echo "$PERSIST_RESPONSE" | grep -q '"status":"success"'; then
    echo "  ✓ User data persisted correctly"
else
    echo "  ✗ Persistence failed"
fi
echo ""

echo "Shutting down server..."
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  Test Complete!"
echo "  Check server.log for detailed server output"
echo "═══════════════════════════════════════════════════════════"