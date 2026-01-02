#!/bin/bash

BASE_URL="http://localhost:8080"
TOKEN=""
USER_ID=""
OPPONENT_ID=""

echo "=========================================="
echo "Chess Platform - Comprehensive Test Suite"
echo "=========================================="
echo ""

test_endpoint() {
    local method=$1
    local endpoint=$2
    local data=$3
    local name=$4
    
    echo "TEST: $name"
    echo "  Method: $method"
    echo "  URL: $BASE_URL$endpoint"
    
    local curl_opts="-s -X $method"
    
    if [ -n "$TOKEN" ]; then
        curl_opts="$curl_opts -H 'Authorization: Bearer $TOKEN'"
    fi
    
    if [ -n "$data" ]; then
        curl_opts="$curl_opts -H 'Content-Type: application/json' -d '$data'"
        echo "  Data: $data"
    fi
    
    curl_opts="$curl_opts $BASE_URL$endpoint"
    
    local response=$(eval "curl $curl_opts")
    echo "  Response: $response"
    echo ""
    
    echo "$response"
}

echo "1. HEALTH CHECK"
test_endpoint "GET" "/health" "" "Health Endpoint"

echo "2. AUTHENTICATION TESTS"
echo ""

REGISTER_RESP=$(test_endpoint "POST" "/auth/register" '{"username":"testuser1","password":"password123"}' "Register User 1")
TOKEN=$(echo $REGISTER_RESP | grep -o '"token":"[^"]*' | head -1 | cut -d'"' -f4)
USER_ID=$(echo $REGISTER_RESP | grep -o '"user_id":[0-9]*' | head -1 | cut -d':' -f2)

echo "User 1 Token: $TOKEN"
echo "User 1 ID: $USER_ID"
echo ""

echo ""
REGISTER_RESP2=$(test_endpoint "POST" "/auth/register" '{"username":"testuser2","password":"password123"}' "Register User 2")
TOKEN2=$(echo $REGISTER_RESP2 | grep -o '"token":"[^"]*' | head -1 | cut -d'"' -f4)
USER_ID2=$(echo $REGISTER_RESP2 | grep -o '"user_id":[0-9]*' | head -1 | cut -d':' -f2)

echo "User 2 Token: $TOKEN2"
echo "User 2 ID: $USER_ID2"
echo ""

echo ""
test_endpoint "POST" "/auth/login" '{"username":"testuser1","password":"password123"}' "Login User"
echo ""

echo "3. USER MANAGEMENT TESTS"
echo ""

test_endpoint "GET" "/user/me" "" "Get Current User"
echo ""

echo "4. LEADERBOARD TESTS"
echo ""

test_endpoint "GET" "/leaderboard" "" "Get Leaderboard"
echo ""

echo "5. SEARCH TESTS"
echo ""

test_endpoint "GET" "/users/search?q=testuser" "" "Search Users"
echo ""

echo "6. MATCHMAKING TESTS"
echo ""

test_endpoint "POST" "/match/queue" "" "Queue for Match"
echo ""

test_endpoint "POST" "/match/find" "" "Find Match"
echo ""

echo "7. MATCH RECORDING TESTS"
echo ""

test_endpoint "POST" "/match/record" "{\"opponent_id\":$USER_ID2,\"winner_id\":$USER_ID}" "Record Win"
echo ""

test_endpoint "GET" "/match/history" "" "Get Match History"
echo ""

echo "8. FRIENDS TESTS"
echo ""

test_endpoint "POST" "/friends/request" "{\"friend_id\":$USER_ID2}" "Send Friend Request"
echo ""

test_endpoint "POST" "/friends/accept" "{\"friend_id\":$USER_ID2}" "Accept Friend Request"
echo ""

test_endpoint "GET" "/friends/recommendations" "" "Get Friend Recommendations"
echo ""

echo "9. LOGOUT TESTS"
echo ""

test_endpoint "POST" "/auth/logout" "" "Logout"
echo ""

echo "=========================================="
echo "Test Suite Complete"
echo "=========================================="
