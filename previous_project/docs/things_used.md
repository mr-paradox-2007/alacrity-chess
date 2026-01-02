# References/Tools I used for this project

## Graphics & Game Engine
- **Raylib**: https://www.raylib.com
  - Used for: Raylib native client (Phase 2+)
  - Features: 2D/3D graphics, input handling, cross-platform

## Networking
- **Raw TCP Sockets**: Standard C++ sockets (`<sys/socket.h>`, `<arpa/inet.h>`)
  - Used for: Direct, low-level communication between the client and server.
  - Features: Provides a reliable, stream-oriented connection for sending JSON-based messages.
  - Status: Implemented in the server for handling authentication and game logic.


## Data Structures (Custom Implementation)
- **Hash Table**: O(1) user lookup & authentication
- **B-Tree**: O(log n) game history & leaderboard queries
- **Graph**: O(1) friend relationship management with BFS
- **Max-Heap**: O(log n) matchmaking queue priority
- **LRU Cache**: O(1) active game state caching with automatic eviction

## Development Environment
- **Language**: C++17 with `-pthread` support
- **Compiler**: g++ with std=c++17
- **Threading**: std::thread, std::mutex, std::lock_guard
- **Logger**: Custom logger utility writing to logs/ folder (Phase 1)
