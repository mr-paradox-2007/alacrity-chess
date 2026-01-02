# Chess Platform

Complete web-based chess platform with advanced data structures and real-time features.

## Building

```bash
./build.sh
```

## Running

```bash
./bin/chess_server
```

Server will listen on port 8080.

## Frontend

Open `client/index.html` in a web browser to access the chess platform UI.

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
