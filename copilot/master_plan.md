# chess platform master plan

## project overview

complete web-based chess platform simulation showcasing advanced data structure implementations with concurrent user handling, real-time updates, and comprehensive match simulation system.

## technology stack

### backend
- pure c++17 with standard library
- posix sockets for networking
- custom data structures only
- multi-threaded architecture
- file-based persistence

### frontend
- vanilla html5
- vanilla javascript (es6+)
- vanilla css3
- no frameworks or libraries
- websocket-like polling for real-time updates

## core data structure usage

### hash table
- user authentication (username -> user_data)
- session management (token -> session_info)
- quick user lookups by username
- o(1) average case operations

### b-tree
- match history storage (timestamp-indexed)
- allows range queries for date filtering
- efficient pagination of results
- stores: player1_id, player2_id, winner, timestamp, elo_change, duration

### graph (adjacency list)
- friend relationships (undirected)
- friend discovery (bfs for friends-of-friends)
- online status tracking
- mutual friends calculation

### max heap
- matchmaking queue (priority by elo + wait time)
- real-time leaderboard top-n queries
- efficient insertion and extraction
- priority formula: 0.6 * elo + 0.4 * wait_time_seconds

### lru cache
- active session data caching
- frequently accessed user profiles
- automatic eviction of stale data
- capacity-based memory management

## project structure

```
chess_platform/
├── server/
│   ├── src/
│   │   ├── core/
│   │   │   ├── hash_table.cpp/hpp
│   │   │   ├── b_tree.cpp/hpp
│   │   │   ├── graph.cpp/hpp
│   │   │   ├── max_heap.cpp/hpp
│   │   │   └── lru_cache.cpp/hpp
│   │   ├── network/
│   │   │   ├── http_server.cpp/hpp
│   │   │   ├── request_parser.cpp/hpp
│   │   │   └── response_builder.cpp/hpp
│   │   ├── api/
│   │   │   ├── auth_handler.cpp/hpp
│   │   │   ├── user_handler.cpp/hpp
│   │   │   ├── friend_handler.cpp/hpp
│   │   │   ├── match_handler.cpp/hpp
│   │   │   └── leaderboard_handler.cpp/hpp
│   │   ├── services/
│   │   │   ├── user_service.cpp/hpp
│   │   │   ├── match_service.cpp/hpp
│   │   │   ├── friend_service.cpp/hpp
│   │   │   └── session_service.cpp/hpp
│   │   ├── models/
│   │   │   ├── user.hpp
│   │   │   ├── match.hpp
│   │   │   ├── session.hpp
│   │   │   └── friend_request.hpp
│   │   └── utils/
│   │       ├── password_hash.cpp/hpp
│   │       ├── json_parser.cpp/hpp
│   │       ├── file_utils.cpp/hpp
│   │       └── time_utils.cpp/hpp
│   ├── data/
│   │   ├── users.dat
│   │   ├── matches.dat
│   │   ├── friends.dat
│   │   └── sessions.dat
│   ├── uploads/
│   │   └── profile_pictures/
│   └── main.cpp
├── client/
│   ├── index.html
│   ├── dashboard.html
│   ├── users.html
│   ├── friends.html
│   ├── leaderboard.html
│   ├── matches.html
│   ├── profile.html
│   ├── css/
│   │   ├── main.css
│   │   ├── auth.css
│   │   └── components.css
│   └── js/
│       ├── api.js
│       ├── auth.js
│       ├── dashboard.js
│       ├── users.js
│       ├── friends.js
│       ├── leaderboard.js
│       ├── matches.js
│       └── profile.js
├── tests/
│   ├── load_test.cpp
│   └── data_generator.cpp
├── build.sh
└── README.md
```

## key features breakdown

### 1. authentication system
- registration with username validation
- secure password hashing (sha256 + salt)
- session token generation
- session expiration handling
- concurrent login prevention
- logout functionality

### 2. user discovery
- browse all users with pagination
- sort by: elo, join_date, username, win_rate
- search by username (prefix matching)
- view user profiles
- efficient queries using hash table + sorted structures

### 3. friend system
- send friend requests
- accept/reject requests
- unfriend functionality
- view friends list
- see online friends
- friend recommendations (friends-of-friends via bfs)
- mutual friends display

### 4. leaderboard
- top players by elo
- filter by: all-time, monthly, weekly
- pagination support
- real-time rank updates
- uses max heap for top-n extraction

### 5. match simulation
- queue for matchmaking
- elo-based pairing (within 200 points)
- simulate match with random outcome
- elo calculation using standard formula
- match duration simulation
- automatic history recording

### 6. match history
- view personal match history
- view other players' history
- filter by: date range, opponent, result
- pagination with b-tree range queries
- replay-style visualization
- detailed statistics

### 7. profile management
- view/edit profile
- upload profile picture (base64 encoding)
- display statistics: elo, win/loss/draw, total matches
- match history timeline
- friends list

### 8. real-time updates
- polling mechanism (every 2-3 seconds)
- update notifications for:
  - friend requests
  - match found
  - elo changes
  - friend online/offline status

## concurrency strategy

### mutex protection
- user_data_mutex: protects hash table operations
- match_history_mutex: protects b-tree operations
- friend_graph_mutex: protects graph operations
- matchmaking_mutex: protects heap operations
- session_cache_mutex: protects lru cache operations

### thread architecture
- main thread: accept connections
- worker pool: handle http requests (10 threads)
- matchmaking thread: process queue every 1 second
- persistence thread: save data every 30 seconds
- cleanup thread: remove expired sessions every 5 minutes

## api endpoints

### authentication
- POST /api/auth/register
- POST /api/auth/login
- POST /api/auth/logout
- GET /api/auth/session

### users
- GET /api/users/search?query=...&sort=...&page=...
- GET /api/users/:username
- PUT /api/users/profile
- POST /api/users/profile/picture

### friends
- GET /api/friends
- POST /api/friends/request
- PUT /api/friends/accept/:username
- DELETE /api/friends/:username
- GET /api/friends/online
- GET /api/friends/recommendations

### matchmaking
- POST /api/matchmaking/queue
- DELETE /api/matchmaking/queue
- GET /api/matchmaking/status

### matches
- GET /api/matches/history?page=...&filter=...
- GET /api/matches/:match_id
- GET /api/matches/user/:username?page=...
- POST /api/matches/simulate

### leaderboard
- GET /api/leaderboard?period=...&page=...
- GET /api/leaderboard/rank/:username

## data persistence format

### users.dat structure
```
[4 bytes: version]
[4 bytes: num_users]
for each user:
  [4 bytes: username_length]
  [username_length bytes: username]
  [64 bytes: password_hash]
  [32 bytes: salt]
  [4 bytes: elo_rating]
  [4 bytes: total_matches]
  [4 bytes: wins]
  [4 bytes: losses]
  [4 bytes: draws]
  [8 bytes: registration_timestamp]
  [8 bytes: last_login_timestamp]
  [4 bytes: profile_picture_length]
  [profile_picture_length bytes: picture_data]
```

### matches.dat structure
```
[4 bytes: version]
[4 bytes: num_matches]
for each match:
  [16 bytes: match_id]
  [4 bytes: player1_username_length]
  [player1_username_length bytes: player1_username]
  [4 bytes: player2_username_length]
  [player2_username_length bytes: player2_username]
  [4 bytes: winner (0=draw, 1=player1, 2=player2)]
  [4 bytes: player1_elo_before]
  [4 bytes: player2_elo_before]
  [4 bytes: player1_elo_after]
  [4 bytes: player2_elo_after]
  [8 bytes: timestamp]
  [4 bytes: duration_seconds]
```

### friends.dat structure
```
[4 bytes: version]
[4 bytes: num_edges]
for each edge:
  [4 bytes: user1_username_length]
  [user1_username_length bytes: user1_username]
  [4 bytes: user2_username_length]
  [user2_username_length bytes: user2_username]
  [8 bytes: friendship_timestamp]
```

### sessions.dat structure
```
[4 bytes: version]
[4 bytes: num_sessions]
for each session:
  [64 bytes: session_token]
  [4 bytes: username_length]
  [username_length bytes: username]
  [8 bytes: creation_timestamp]
  [8 bytes: expiration_timestamp]
  [1 byte: is_active]
```

## testing strategy

### unit tests
- each data structure operation
- password hashing/verification
- elo calculation
- json parsing
- session management

### integration tests
- complete registration -> login -> logout flow
- friend request -> accept flow
- matchmaking -> match simulation flow
- match history queries with pagination
- concurrent user operations

### load tests
- 1000 concurrent user registrations
- 500 simultaneous matchmaking requests
- 10000 match history queries
- rapid friend request spam
- profile picture uploads (100MB total)

### data generation
- script to generate 10000 fake users
- script to generate 50000 fake matches
- script to generate random friend connections
- benchmark queries against large datasets

## performance targets

- user lookup: < 1ms
- match history query: < 10ms for 100 results
- friend recommendations: < 50ms
- matchmaking: < 2s average wait for pair
- leaderboard top 100: < 5ms
- concurrent requests: 500 req/sec sustained

## security considerations

- password hashing with unique salts
- session token entropy (32+ bytes random)
- sql injection prevention (no sql, but input validation)
- file upload size limits (5MB per picture)
- rate limiting per ip (100 req/min)
- xss prevention in all outputs

## development phases

### phase 1: core infrastructure (days 1-2)
- implement all 5 data structures
- write unit tests for each
- implement persistence layer
- test serialization/deserialization

### phase 2: http server (days 2-3)
- basic http server with thread pool
- request parsing
- response building
- routing system
- static file serving

### phase 3: authentication (days 3-4)
- user registration
- password hashing
- login/logout
- session management
- frontend auth pages

### phase 4: user system (days 4-5)
- user discovery
- profile viewing
- profile editing
- search and sorting
- frontend user pages

### phase 5: friend system (days 5-6)
- friend requests
- acceptance/rejection
- friend list
- recommendations
- frontend friend pages

### phase 6: matchmaking (days 6-7)
- queue implementation
- pairing algorithm
- match simulation
- elo calculation
- frontend match pages

### phase 7: history & leaderboard (days 7-8)
- history storage
- history queries
- leaderboard generation
- pagination
- frontend display pages

### phase 8: real-time updates (day 8)
- polling system
- notification system
- status updates
- frontend integration

### phase 9: testing & optimization (days 9-10)
- load testing
- data generation
- bug fixes
- performance tuning
- documentation

## deliverables

1. compiled server executable
2. complete web client
3. test suite with results
4. load test results with 10000 users
5. demonstration script
6. architecture documentation
7. viva presentation materials

## viva preparation

### demonstrate understanding of:
1. hash table collision resolution
2. b-tree balancing and range queries
3. graph bfs for recommendations
4. heap heapify operations
5. lru cache eviction policy

### showcase implementations:
1. concurrent access handling with mutexes
2. efficient memory management
3. file i/o for persistence
4. network programming with sockets
5. multi-threaded server architecture

### performance demonstrations:
1. 10000 user lookups in < 10ms
2. friend recommendations in < 50ms
3. match history pagination speed
4. real-time matchmaking efficiency
5. leaderboard query performance

## critical success factors

1. all data structures must be custom implementations
2. thread-safe operations throughout
3. fast response times under load
4. data persists across server restarts
5. clean, readable code following conventions
6. comprehensive error handling
7. smooth user experience
8. accurate data structure complexity analysis
