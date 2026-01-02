# Chess Platform

Complete web-based chess platform with advanced data structures and real-time features.

## Quick Start

```bash
# Build and start the server
./start_server.sh
```

Or manually:

```bash
# Build the server
./build.sh

# Run the server
./bin/chess_server
```

Server will listen on **port 8080**.

## Accessing the Application

Once the server is running, open your web browser and navigate to:

**http://localhost:8080**

The server will automatically serve the frontend application.

## Features

- User registration and authentication
- Session management with tokens
- User dashboard with stats
- Leaderboard
- Friend system (graph-based)
- Matchmaking queue (max heap)
- Match history (B-tree)

## Architecture

### Data Structures
- **Hash Table**: User authentication and session management (O(1) operations)
- **B-Tree**: Match history storage with range queries for date filtering
- **Graph**: Friend system with BFS for recommendations
- **Max Heap**: Matchmaking queue and leaderboard
- **LRU Cache**: Active session data caching

### API Endpoints
- `POST /auth/register` - Register new user
- `POST /auth/login` - Login user
- `POST /auth/logout` - Logout user
- `GET /user/me` - Get current user info
- `GET /leaderboard` - Get top players

## Testing

Run data structure tests:
```bash
./tests/ds_test
```

Run load test with 1000 users:
```bash
./tests/load_test
```

## Performance
- 1000 user registration: ~6.4s
- 1000 user login: ~4.8s
- 500 friend connections: ~1ms

## Technology
- Pure C++17 (no external libraries)
- POSIX sockets for networking
- Multi-threaded with mutex protection
- Vanilla HTML5/CSS3/JavaScript frontend
