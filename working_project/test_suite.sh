#!/bin/bash

BASE_URL="http://localhost:8080"
TOKEN=""
USER_ID=""

echo "=========================================="
echo "Chess Platform - API Test Suite"
echo "=========================================="
echo ""

# Test 1: Health
echo "✓ TEST 1: Health Check"
HEALTH=$(curl -s -X GET "$BASE_URL/health")
echo "  Response: $HEALTH"
echo ""

# Test 2: Register User 1
echo "✓ TEST 2: Register User 1"
REG1=$(curl -s -X POST "$BASE_URL/auth/register" \
  -H "Content-Type: application/json" \
  -d '{"username":"alice","password":"password123"}')
echo "  Response: $REG1"
TOKEN=$(echo "$REG1" | grep -o '"token":"[^"]*"' | cut -d'"' -f4)
USER_ID=$(echo "$REG1" | grep -o '"user_id":[0-9]*' | cut -d':' -f2)
echo "  Token: $TOKEN"
echo "  User ID: $USER_ID"
echo ""

# Test 3: Register User 2
echo "✓ TEST 3: Register User 2"
REG2=$(curl -s -X POST "$BASE_URL/auth/register" \
  -H "Content-Type: application/json" \
  -d '{"username":"bob","password":"password123"}')
echo "  Response: $REG2"
TOKEN2=$(echo "$REG2" | grep -o '"token":"[^"]*"' | cut -d'"' -f4)
USER_ID2=$(echo "$REG2" | grep -o '"user_id":[0-9]*' | cut -d':' -f2)
echo "  Token: $TOKEN2"
echo "  User ID: $USER_ID2"
echo ""

# Test 4: Get User Profile
echo "✓ TEST 4: Get User Profile"
PROFILE=$(curl -s -X GET "$BASE_URL/user/me" \
  -H "Authorization: Bearer $TOKEN")
echo "  Response: $PROFILE"
echo ""

# Test 5: Get Leaderboard
echo "✓ TEST 5: Get Leaderboard"
LEADERBOARD=$(curl -s -X GET "$BASE_URL/leaderboard")
echo "  Response: $LEADERBOARD"
echo ""

# Test 6: Search Users
echo "✓ TEST 6: Search Users"
SEARCH=$(curl -s -X GET "$BASE_URL/users/search?q=alice")
echo "  Response: $SEARCH"
echo ""

# Test 7: Queue for Match (User 1)
echo "✓ TEST 7: Queue for Match (User 1)"
QUEUE1=$(curl -s -X POST "$BASE_URL/match/queue" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{}')
echo "  Response: $QUEUE1"
echo ""

# Test 8: Queue for Match (User 2)
echo "✓ TEST 8: Queue for Match (User 2)"
QUEUE2=$(curl -s -X POST "$BASE_URL/match/queue" \
  -H "Authorization: Bearer $TOKEN2" \
  -H "Content-Type: application/json" \
  -d '{}')
echo "  Response: $QUEUE2"
echo ""

# Test 9: Find Match for User 1
echo "✓ TEST 9: Find Match for User 1"
FIND_MATCH=$(curl -s -X POST "$BASE_URL/match/find" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{}')
echo "  Response: $FIND_MATCH"
OPPONENT_ID=$(echo "$FIND_MATCH" | grep -o '"opponent_id":[0-9]*' | cut -d':' -f2)
echo ""

# Test 10: Record Match Result
echo "✓ TEST 10: Record Match Result (User 1 wins)"
RECORD=$(curl -s -X POST "$BASE_URL/match/record" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d "{\"opponent_id\":$OPPONENT_ID,\"winner_id\":$USER_ID}")
echo "  Response: $RECORD"
echo ""

# Test 11: Get Match History
echo "✓ TEST 11: Get Match History"
HISTORY=$(curl -s -X GET "$BASE_URL/match/history" \
  -H "Authorization: Bearer $TOKEN")
echo "  Response: $HISTORY"
echo ""

# Test 12: Send Friend Request
echo "✓ TEST 12: Send Friend Request"
FRIEND_REQ=$(curl -s -X POST "$BASE_URL/friends/request" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d "{\"friend_id\":$USER_ID2}")
echo "  Response: $FRIEND_REQ"
echo ""

# Test 13: Accept Friend Request
echo "✓ TEST 13: Accept Friend Request"
ACCEPT_REQ=$(curl -s -X POST "$BASE_URL/friends/accept" \
  -H "Authorization: Bearer $TOKEN2" \
  -H "Content-Type: application/json" \
  -d "{\"friend_id\":$USER_ID}")
echo "  Response: $ACCEPT_REQ"
echo ""

# Test 14: Get Friend Recommendations
echo "✓ TEST 14: Get Friend Recommendations"
RECOMMENDATIONS=$(curl -s -X GET "$BASE_URL/friends/recommendations" \
  -H "Authorization: Bearer $TOKEN")
echo "  Response: $RECOMMENDATIONS"
echo ""

# Test 15: Login
echo "✓ TEST 15: Login"
LOGIN=$(curl -s -X POST "$BASE_URL/auth/login" \
  -H "Content-Type: application/json" \
  -d '{"username":"alice","password":"password123"}')
echo "  Response: $LOGIN"
echo ""

# Test 16: Logout
echo "✓ TEST 16: Logout"
LOGOUT=$(curl -s -X POST "$BASE_URL/auth/logout" \
  -H "Authorization: Bearer $TOKEN")
echo "  Response: $LOGOUT"
echo ""

echo "=========================================="
echo "✓ All Tests Completed"
echo "=========================================="
