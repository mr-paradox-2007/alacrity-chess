# References/Tools I used for this project

## Graphics & Game Engine
- **Raylib**: https://www.raylib.com
  - Used for: Raylib native client (Phase 2+)
  - Features: 2D/3D graphics, input handling, cross-platform

## Networking
- **uWebSockets**: https://github.com/uNetworking/uWebSockets
  - Used for: WebSocket server for real-time multiplayer
  - Installed: `/usr/local/include/uWebSockets/`
  - Version: Header-only C++ library
  - Features: Fast, lightweight, production-ready WebSocket support
  - Installation Date: December 31, 2025

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
