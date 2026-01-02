#!/bin/bash

echo "=========================================="
echo "  Chess Platform Feature Test Suite"
echo "=========================================="
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Kill any existing server
pkill -f chess_server 2>/dev/null || true
sleep 1

# Start server
echo "Starting server..."
./bin/chess_server > /tmp/server_test.log 2>&1 &
SERVER_PID=$!
sleep 2

# Test counter
TESTS_PASSED=0
TESTS_FAILED=0

test_endpoint() {
    local name=$1
    local method=$2
    local endpoint=$3
    local data=$4
    local expected=$5
    
    echo -n "Testing $name... "
    
    if [ "$method" = "GET" ]; then
        response=$(curl -s -w "\n%{http_code}" http://localhost:8080$endpoint)
    else
        response=$(curl -s -w "\n%{http_code}" -X $method \
            -H "Content-Type: application/json" \
            -d "$data" \
            http://localhost:8080$endpoint)
    fi
    
    http_code=$(echo "$response" | tail -1)
    body=$(echo "$response" | head -n -1)
    
    if [ "$http_code" = "200" ] || echo "$body" | grep -q "$expected"; then
        echo -e "${GREEN}✓ PASSED${NC}"
        ((TESTS_PASSED++))
        return 0
    else
        echo -e "${RED}✗ FAILED${NC}"
        echo "  Response: $body"
        echo "  HTTP Code: $http_code"
        ((TESTS_FAILED++))
        return 1
    fi
}

# Test 1: Health check
test_endpoint "Health Check" "GET" "/health" "" "ok"

# Test 2: Register user
REGISTER_RESPONSE=$(curl -s -X POST \
    -H "Content-Type: application/json" \
    -d '{"username":"testuser'$(date +%s)'","password":"testpass123"}' \
    http://localhost:8080/auth/register)

TOKEN=$(echo $REGISTER_RESPONSE | grep -o '"token":"[^"]*' | cut -d'"' -f4)

if [ -n "$TOKEN" ]; then
    echo -e "User Registration: ${GREEN}✓ PASSED${NC}"
    ((TESTS_PASSED++))
else
    echo -e "User Registration: ${RED}✗ FAILED${NC}"
    ((TESTS_FAILED++))
fi

# Test 3: Get user info
if [ -n "$TOKEN" ]; then
    USER_RESPONSE=$(curl -s -H "Authorization: Bearer $TOKEN" \
        http://localhost:8080/user/me)
    
    if echo "$USER_RESPONSE" | grep -q "elo"; then
        echo -e "Get User Info: ${GREEN}✓ PASSED${NC}"
        ((TESTS_PASSED++))
    else
        echo -e "Get User Info: ${RED}✗ FAILED${NC}"
        ((TESTS_FAILED++))
    fi
fi

# Test 4: Queue for match
if [ -n "$TOKEN" ]; then
    QUEUE_RESPONSE=$(curl -s -X POST \
        -H "Authorization: Bearer $TOKEN" \
        -H "Content-Type: application/json" \
        http://localhost:8080/match/queue)
    
    if echo "$QUEUE_RESPONSE" | grep -q "ok"; then
        echo -e "Queue for Match: ${GREEN}✓ PASSED${NC}"
        ((TESTS_PASSED++))
    else
        echo -e "Queue for Match: ${RED}✗ FAILED${NC}"
        ((TESTS_FAILED++))
    fi
fi

# Test 5: Leaderboard
LEADERBOARD_RESPONSE=$(curl -s http://localhost:8080/leaderboard)
if echo "$LEADERBOARD_RESPONSE" | grep -q "leaderboard"; then
    echo -e "Leaderboard: ${GREEN}✓ PASSED${NC}"
    ((TESTS_PASSED++))
else
    echo -e "Leaderboard: ${RED}✗ FAILED${NC}"
    ((TESTS_FAILED++))
fi

# Test 6: Match History
if [ -n "$TOKEN" ]; then
    HISTORY_RESPONSE=$(curl -s -H "Authorization: Bearer $TOKEN" \
        http://localhost:8080/match/history)
    
    if echo "$HISTORY_RESPONSE" | grep -q "matches"; then
        echo -e "Match History: ${GREEN}✓ PASSED${NC}"
        ((TESTS_PASSED++))
    else
        echo -e "Match History: ${RED}✗ FAILED${NC}"
        ((TESTS_FAILED++))
    fi
fi

# Cleanup
kill $SERVER_PID 2>/dev/null || true

echo ""
echo "=========================================="
echo "  Test Results"
echo "=========================================="
echo -e "${GREEN}Passed: $TESTS_PASSED${NC}"
echo -e "${RED}Failed: $TESTS_FAILED${NC}"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed! ✓${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed! ✗${NC}"
    exit 1
fi

