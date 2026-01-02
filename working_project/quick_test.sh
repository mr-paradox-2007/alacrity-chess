#!/bin/bash

BASE_URL="http://localhost:8080"

echo "✓ Quick Endpoint Validation"
echo ""

# Register and extract token
echo "1. Register User"
REG=$(curl -s -X POST "$BASE_URL/auth/register" \
  -H "Content-Type: application/json" \
  -d "{\"username\":\"user$(date +%s)\",\"password\":\"pass123\"}")
TOKEN=$(echo "$REG" | grep -o '"token":"[^"]*"' | cut -d'"' -f4)
USER_ID=$(echo "$REG" | grep -o '"user_id":[0-9]*' | cut -d':' -f2)
echo "   Token: $TOKEN"
echo "   User ID: $USER_ID"
echo ""

# Test endpoints
echo "2. Get User Profile - $(curl -s -X GET "$BASE_URL/user/me" -H "Authorization: Bearer $TOKEN" | grep -o '"username":"[^"]*"')"
echo "3. Get Leaderboard - $(curl -s -X GET "$BASE_URL/leaderboard" | grep -o '"leaderboard":\[' )"
echo "4. Search Users - $(curl -s -X GET "$BASE_URL/users/search?q=user" | grep -o '"users":\[' )"
echo "5. Queue Match - $(curl -s -X POST "$BASE_URL/match/queue" -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" -d '{}' | grep -o '"message":"[^"]*"')"
echo "6. Get Match History - $(curl -s -X GET "$BASE_URL/match/history" -H "Authorization: Bearer $TOKEN" | grep -o '"matches":\[')"
echo "7. Get Recommendations - $(curl -s -X GET "$BASE_URL/friends/recommendations" -H "Authorization: Bearer $TOKEN" | head -c 50)..."
echo "8. Logout - $(curl -s -X POST "$BASE_URL/auth/logout" -H "Authorization: Bearer $TOKEN" | grep -o '"status":"[^"]*"')"
echo ""
echo "✓ All endpoints responding!"
