# step-by-step implementation order

## preparation phase

### step 1: create project structure

create new directory: `chess_platform_final/`

create all directories:
```bash
mkdir -p server/src/core
mkdir -p server/src/network
mkdir -p server/src/api
mkdir -p server/src/services
mkdir -p server/src/models
mkdir -p server/src/utils
mkdir -p server/data
mkdir -p server/uploads/profile_pictures
mkdir -p client/css
mkdir -p client/js
mkdir -p tests
```

create empty files for all modules as listed in architecture documents.

## phase 1: core data structures (priority 1)

### step 2: implement hash_table

file: `server/src/core/hash_table.hpp`

```cpp
#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <string>
#include <mutex>
#include <functional>
#include <cstdint>

template<typename K, typename V>
class hash_table {
private:
    struct node {
        K key;
        V value;
        node* next;
        node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };
    
    node** buckets;
    size_t capacity;
    size_t count;
    mutable std::mutex mutex;
    
    size_t hash_function(const K& key) const;
    void resize();
    
public:
    hash_table(size_t initial_capacity = 1024);
    ~hash_table();
    
    bool insert(const K& key, const V& value);
    bool find(const K& key, V& value) const;
    bool remove(const K& key);
    bool update(const K& key, const V& value);
    size_t size() const;
    bool empty() const;
    
    void serialize(const std::string& filename) const;
    void deserialize(const std::string& filename);
    
    void iterate(void (*callback)(const K&, const V&));
};

#endif
```

file: `server/src/core/hash_table.cpp`

implement all methods following standard chaining collision resolution.

critical details:
- use std::hash<K> for hash function
- resize when load factor > 0.75
- double capacity on resize
- rehash all elements after resize
- mutex lock for thread safety

### step 3: implement b_tree

file: `server/src/core/b_tree.hpp`

order 5 b-tree with standard insert, remove, range_query operations.

file: `server/src/core/b_tree.cpp`

implement:
- split_child when node full
- merge when underflow
- range_query for timestamp filtering

### step 4: implement graph

file: `server/src/core/graph.hpp`

adjacency list representation.

file: `server/src/core/graph.cpp`

implement:
- add_vertex, add_edge, remove_edge
- bfs_recommendations (depth 2 search)
- mutual_friends calculation
- online status tracking

### step 5: implement max_heap

file: `server/src/core/max_heap.hpp`

binary max heap with heapify operations.

file: `server/src/core/max_heap.cpp`

implement:
- insert with heapify_up
- extract_max with heapify_down
- get_top_n for leaderboard

### step 6: implement lru_cache

file: `server/src/core/lru_cache.hpp`

doubly linked list + hash map.

file: `server/src/core/lru_cache.cpp`

implement:
- get with move_to_front
- put with eviction when full
- efficient node removal

### step 7: test all data structures

create: `tests/data_structures_test.cpp`

test each structure independently:
- hash_table: insert 10000 items, test collisions
- b_tree: range queries on 5000 records
- graph: bfs on 1000 node graph
- max_heap: extract operations maintaining heap property
- lru_cache: eviction policy correctness

compile and run:
```bash
g++ -std=c++17 -pthread -o tests/ds_test tests/data_structures_test.cpp \
    server/src/core/hash_table.cpp \
    server/src/core/b_tree.cpp \
    server/src/core/graph.cpp \
    server/src/core/max_heap.cpp \
    server/src/core/lru_cache.cpp
./tests/ds_test
```

## phase 2: models and utilities (priority 2)

### step 8: define data models

file: `server/src/models/user.hpp`

```cpp
#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <cstdint>

struct user_data {
    uint64_t user_id;
    std::string username;
    std::string password_hash;
    std::string salt;
    int elo_rating;
    int total_matches;
    int wins;
    int losses;
    int draws;
    uint64_t registration_timestamp;
    uint64_t last_login_timestamp;
    std::vector<uint8_t> profile_picture;
    bool is_online;
};

struct user_stats {
    int elo_rating;
    int total_matches;
    int wins;
    int losses;
    int draws;
    double win_rate;
};

#endif
```

file: `server/src/models/match.hpp`

define match_data and matchmaking_entry structs.

file: `server/src/models/session.hpp`

define session_data struct.

### step 9: implement password hashing

file: `server/src/utils/password_hash.hpp`
file: `server/src/utils/password_hash.cpp`

implement:
- generate_salt using /dev/urandom
- hash_password using sha256
- verify_password by comparing hashes

use openssl or write simple sha256 implementation.

### step 10: implement json utilities

file: `server/src/utils/json_parser.hpp`
file: `server/src/utils/json_parser.cpp`

manual json parsing (no libraries):
- extract_string: find "key":"value" pattern
- extract_int: parse number after colon
- build_object: create json from map
- build_array: create json array

### step 11: implement file utilities

file: `server/src/utils/file_utils.hpp`
file: `server/src/utils/file_utils.cpp`

implement:
- read_binary_file
- write_binary_file
- file_exists
- create_directory_if_not_exists

### step 12: implement time utilities

file: `server/src/utils/time_utils.hpp`
file: `server/src/utils/time_utils.cpp`

implement:
- get_current_timestamp
- format_timestamp
- timestamp_to_string

## phase 3: network layer (priority 3)

### step 13: implement http request parser

file: `server/src/network/request_parser.hpp`
file: `server/src/network/request_parser.cpp`

parse raw http request string into structured http_request object:
- extract method (get, post, put, delete)
- extract path and query string
- parse headers
- extract body
- parse query parameters

### step 14: implement http response builder

file: `server/src/network/response_builder.hpp`
file: `server/src/network/response_builder.cpp`

build http response string:
- format status line
- add headers (content-type, content-length, cors)
- append body
- helper for json responses

### step 15: implement http server

file: `server/src/network/http_server.hpp`
file: `server/src/network/http_server.cpp`

multi-threaded http server:
- create socket and bind to port
- listen for connections
- accept loop in main thread
- worker thread pool (10 threads)
- each worker handles one request
- route requests to handlers
- send responses

critical implementation details:
```cpp
http_server::http_server(int port) : port(port), running(false) {
    server_socket = socket(af_inet, sock_stream, 0);
    
    int opt = 1;
    setsockopt(server_socket, sol_socket, so_reuseaddr, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = af_inet;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inaddr_any;
    
    bind(server_socket, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_socket, 100);
}

void http_server::worker_loop() {
    while (running) {
        int client = accept(server_socket, nullptr, nullptr);
        
        char buffer[65536];
        int bytes = recv(client, buffer, sizeof(buffer), 0);
        
        http_request req = request_parser::parse(std::string(buffer, bytes));
        http_response res = route_request(req);
        std::string response_str = response_builder::build(res);
        
        send(client, response_str.c_str(), response_str.length(), 0);
        close(client);
    }
}
```

### step 16: test http server

create simple test:
```bash
curl http://localhost:8080/test
```

should return 404 with json error.

## phase 4: business logic layer (priority 4)

### step 17: implement user service

file: `server/src/services/user_service.hpp`
file: `server/src/services/user_service.cpp`

implement:
- create_user: validate, hash password, insert into hash_table
- find_user: lookup in hash_table
- update_user: modify existing user data
- get_stats: calculate win rate and statistics
- search_users: iterate hash_table, filter, sort

### step 18: implement session service

file: `server/src/services/session_service.hpp`
file: `server/src/services/session_service.cpp`

implement:
- create_session: generate random token, store in hash_table and lru_cache
- validate_session: check token exists and not expired
- invalidate_session: remove from both stores
- cleanup_expired_sessions: periodic task to remove old sessions

### step 19: implement friend service

file: `server/src/services/friend_service.hpp`
file: `server/src/services/friend_service.cpp`

implement:
- add_friend: add edge in graph
- remove_friend: remove edge
- get_friends: get neighbors from graph
- get_online_friends: filter by online status
- get_recommendations: bfs on graph
- get_mutual_friends: intersection of friend lists

### step 20: implement match service

file: `server/src/services/match_service.hpp`
file: `server/src/services/match_service.cpp`

implement:
- simulate_match: random outcome, calculate elo
- record_match: insert into b_tree
- get_history: range query on b_tree
- get_recent_matches: query last n matches

elo calculation:
```cpp
double expected_score = 1.0 / (1.0 + pow(10.0, (opponent_elo - player_elo) / 400.0));
double actual_score = result == win ? 1.0 : (result == draw ? 0.5 : 0.0);
int elo_change = (int)(k_factor * (actual_score - expected_score));
```

## phase 5: api handlers (priority 5)

### step 21: implement auth handler

file: `server/src/api/auth_handler.hpp`
file: `server/src/api/auth_handler.cpp`

implement:
- handle_register: parse json, validate, create user, return success
- handle_login: verify credentials, create session, return token
- handle_logout: invalidate session
- handle_session_check: validate token, return user data

### step 22: implement user handler

file: `server/src/api/user_handler.hpp`
file: `server/src/api/user_handler.cpp`

implement:
- handle_search: query params for search/sort/pagination
- handle_get_profile: return user data
- handle_update_profile: modify user fields
- handle_upload_picture: decode base64, save to file

### step 23: implement friend handler

file: `server/src/api/friend_handler.hpp`
file: `server/src/api/friend_handler.cpp`

implement:
- handle_send_request: add friendship
- handle_list_friends: get all friends
- handle_recommendations: bfs search
- handle_online: filter online friends

### step 24: implement match handler

file: `server/src/api/match_handler.hpp`
file: `server/src/api/match_handler.cpp`

implement:
- handle_simulate_match: random opponent or specific
- handle_history: paginated match list
- handle_get_match: single match details

### step 25: implement leaderboard handler

file: `server/src/api/leaderboard_handler.hpp`
file: `server/src/api/leaderboard_handler.cpp`

implement:
- handle_get_leaderboard: extract top n from heap
- handle_get_rank: find user position in sorted list

## phase 6: main server integration (priority 6)

### step 26: implement main.cpp

file: `server/src/main.cpp`

```cpp
#include "network/http_server.hpp"
#include "services/user_service.hpp"
#include "services/session_service.hpp"
#include "services/friend_service.hpp"
#include "services/match_service.hpp"
#include "api/auth_handler.hpp"
#include "api/user_handler.hpp"
#include "api/friend_handler.hpp"
#include "api/match_handler.hpp"
#include "api/leaderboard_handler.hpp"
#include <signal.h>

hash_table<std::string, user_data> user_database;
hash_table<std::string, session_data> session_store;
b_tree<uint64_t, match_data> match_history;
graph friend_graph;
max_heap<matchmaking_entry> matchmaking_queue;
lru_cache<std::string, session_data> session_cache(10000);

http_server* server = nullptr;

void signal_handler(int sig) {
    if (server) {
        server->stop();
    }
}

int main(int argc, char* argv[]) {
    int port = argc > 1 ? std::stoi(argv[1]) : 8080;
    
    signal(sigint, signal_handler);
    signal(sigterm, signal_handler);
    
    user_database.deserialize("server/data/users.dat");
    match_history.deserialize("server/data/matches.dat");
    friend_graph.deserialize("server/data/friends.dat");
    session_store.deserialize("server/data/sessions.dat");
    
    server = new http_server(port);
    
    server->start();
    
    user_database.serialize("server/data/users.dat");
    match_history.serialize("server/data/matches.dat");
    friend_graph.serialize("server/data/friends.dat");
    session_store.serialize("server/data/sessions.dat");
    
    delete server;
    
    return 0;
}
```

### step 27: implement routing

in http_server::route_request:

```cpp
http_response route_request(const http_request& req) {
    if (req.path == "/api/auth/register") {
        return auth_handler.handle_register(req);
    }
    else if (req.path == "/api/auth/login") {
        return auth_handler.handle_login(req);
    }
    else if (req.path.find("/api/users/") == 0) {
        return user_handler.handle(req);
    }
    else if (req.path == "/") {
        return response_builder::file_response("client/index.html");
    }
    else if (req.path.find("/css/") == 0 || req.path.find("/js/") == 0) {
        return response_builder::file_response("client" + req.path);
    }
    else {
        return response_builder::error_response(404, "not found");
    }
}
```

### step 28: build and test server

create: `build.sh`

```bash
#!/bin/bash

g++ -std=c++17 -pthread -o server/chess_server \
    server/src/core/*.cpp \
    server/src/network/*.cpp \
    server/src/api/*.cpp \
    server/src/services/*.cpp \
    server/src/utils/*.cpp \
    server/src/main.cpp \
    -Iserver/src \
    -lssl -lcrypto

echo "build complete"
```

test:
```bash
./build.sh
./server/chess_server 8080
```

## phase 7: frontend implementation (priority 7)

### step 29: create base html templates

start with `client/index.html` using structure from frontend architecture.

add all css files in order:
1. reset.css
2. variables.css
3. layout.css
4. components.css
5. auth.css

### step 30: implement javascript core modules

implement in order:
1. config.js
2. api.js
3. storage.js
4. utils.js
5. auth.js

test api communication:
```javascript
api_client.post('/api/auth/register', {username: 'test', password: 'test123'})
```

### step 31: implement auth page

file: `client/js/auth_page.js`

implement tab switching and form handlers.

test registration and login flow manually.

### step 32: implement dashboard

file: `client/dashboard.html`
file: `client/js/dashboard_page.js`

implement stats display, quick match, and polling.

### step 33: implement users page

file: `client/users.html`
file: `client/js/users_page.js`

implement search, sorting, pagination.

### step 34: implement friends page

file: `client/friends.html`
file: `client/js/friends_page.js`

implement friend list, requests, recommendations.

### step 35: implement leaderboard page

file: `client/leaderboard.html`
file: `client/js/leaderboard_page.js`

implement ranking table with sorting.

### step 36: implement matches page

file: `client/matches.html`
file: `client/js/matches_page.js`

implement match history with filtering.

### step 37: implement profile page

file: `client/profile.html`
file: `client/js/profile_page.js`

implement profile viewing and editing.

## phase 8: testing and optimization (priority 8)

### step 38: create load test

file: `tests/load_test.cpp`

generate 10000 users:
```cpp
for (int i = 0; i < 10000; i++) {
    user_data user;
    user.username = "user_" + std::to_string(i);
    user.elo_rating = 1000 + (rand() % 1000);
    user_database.insert(user.username, user);
}
```

measure insertion time, lookup time, range query time.

### step 39: create data generator

file: `tests/data_generator.cpp`

generate fake data:
- 10000 users with random stats
- 50000 matches between random users
- random friend connections (avg 20 friends per user)

save to data files.

### step 40: stress test concurrent access

spawn multiple threads making simultaneous requests:
- 100 threads registering users
- 100 threads simulating matches
- 100 threads querying history

verify no race conditions or deadlocks.

### step 41: profile performance

use gprof or valgrind to identify bottlenecks.

optimize critical paths:
- hash table resize
- b-tree rebalancing
- graph bfs
- heap heapify

### step 42: memory leak detection

run with valgrind:
```bash
valgrind --leak-check=full ./server/chess_server 8080
```

fix all memory leaks.

## phase 9: final polish (priority 9)

### step 43: add error handling

ensure all api endpoints return proper error codes and messages.

add try-catch blocks in all handlers.

log errors to file.

### step 44: add input validation

validate all user inputs:
- username format
- password strength
- file upload sizes
- request body sizes

### step 45: add rate limiting

implement simple rate limiter:
- track requests per ip
- limit to 100 requests per minute
- return 429 too many requests

### step 46: optimize frontend

minify css and javascript.

lazy load images.

implement infinite scroll for long lists.

add loading spinners.

### step 47: create documentation

write README.md with:
- project description
- build instructions
- usage examples
- api documentation
- architecture overview

### step 48: prepare viva materials

create presentation covering:
- data structure implementations
- complexity analysis
- concurrency handling
- performance benchmarks
- demonstration screenshots

## deployment checklist

1. all data structures implemented and tested
2. all api endpoints working
3. frontend pages functional
4. no memory leaks
5. no race conditions
6. load test passes with 10000 users
7. stress test passes with 500 concurrent requests
8. documentation complete
9. viva presentation ready

## build commands summary

compile server:
```bash
g++ -std=c++17 -pthread -o server/chess_server \
    server/src/core/*.cpp \
    server/src/network/*.cpp \
    server/src/api/*.cpp \
    server/src/services/*.cpp \
    server/src/utils/*.cpp \
    server/src/main.cpp \
    -Iserver/src \
    -lssl -lcrypto
```

compile tests:
```bash
g++ -std=c++17 -pthread -o tests/load_test \
    tests/load_test.cpp \
    server/src/core/*.cpp \
    -Iserver/src

g++ -std=c++17 -pthread -o tests/data_gen \
    tests/data_generator.cpp \
    server/src/core/*.cpp \
    -Iserver/src
```

run server:
```bash
./server/chess_server 8080
```

run tests:
```bash
./tests/load_test
./tests/data_gen
```

## success criteria

backend:
- handles 500 concurrent requests
- user lookup < 1ms
- match history query < 10ms
- friend recommendations < 50ms
- no crashes under load
- data persists across restarts

frontend:
- smooth page transitions
- responsive ui updates
- real-time polling works
- all features functional
- mobile responsive
- cross-browser compatible

this completes the step-by-step implementation plan. follow this order precisely for best results.