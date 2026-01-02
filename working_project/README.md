# Chess Platform - Complete Implementation

## Quick Start

```bash
# Build the server (creates binaries in both locations)
./build_server.sh

# Run from working_project directory
./bin/chess_server

# OR run from server/bin directory
./server/bin/chess_server
```

Then open **http://localhost:8080** in your browser.

## Features

✅ **User Authentication**
- Registration and login
- Session management with tokens
- User stats (Elo, wins, losses, draws)

✅ **Matchmaking** (Uses Max Heap)
- Queue for match
- Find opponent by Elo rating
- Priority-based matching

✅ **Leaderboard** (Uses Hash Table + Sorting)
- Top 50 players sorted by Elo
- Shows wins, losses, matches

✅ **Match History** (Uses B-Tree)
- View all your matches
- Shows opponent names and results
- Elo changes displayed

✅ **Match Recording**
- Record win/loss/draw
- Updates Elo ratings
- Updates user statistics

✅ **Friend System** (Uses Graph)
- Search users
- Send friend requests
- Friend recommendations

## Data Structures Used

1. **Hash Table** - User storage, sessions, O(1) lookups
2. **B-Tree** - Match history with range queries
3. **Max Heap** - Matchmaking queue priority
4. **Graph** - Friend connections and recommendations
5. **LRU Cache** - Session data caching

## API Endpoints

- `POST /auth/register` - Register new user
- `POST /auth/login` - Login user
- `POST /auth/logout` - Logout user
- `GET /user/me` - Get current user info
- `GET /leaderboard` - Get top players
- `GET /users/search?q=query` - Search users
- `POST /match/queue` - Queue for matchmaking
- `POST /match/find` - Find opponent
- `GET /match/history` - Get match history
- `POST /match/record` - Record match result
- `POST /friends/request` - Send friend request
- `GET /friends/recommendations` - Get recommendations

## Testing

All features are working and tested. The server automatically:
- Finds the correct client directory
- Handles path resolution from any location
- Returns proper JSON responses
- Uses all data structures correctly

## Project Structure

```
working_project/
├── server/
│   ├── bin/chess_server    # Server executable
│   ├── main.cpp            # Server entry point
│   └── src/                # Source files
├── client/                 # Frontend files
├── bin/chess_server        # Server executable (alternative)
└── build_server.sh         # Build script
```

## Notes

- Server works from both `bin/chess_server` and `server/bin/chess_server`
- Client path is automatically detected
- All data structures are properly implemented and used
- Console logging added for debugging
