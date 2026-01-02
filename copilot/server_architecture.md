# server architecture detailed implementation

## directory structure with file purposes

```
server/
├── src/
│   ├── core/                          [custom data structures]
│   │   ├── hash_table.hpp             [template class for generic key-value storage]
│   │   ├── hash_table.cpp             [implementation with chaining collision resolution]
│   │   ├── b_tree.hpp                 [order-5 b-tree for sorted data]
│   │   ├── b_tree.cpp                 [insertion, deletion, range query implementation]
│   │   ├── graph.hpp                  [adjacency list representation]
│   │   ├── graph.cpp                  [bfs, dfs, friend recommendation algorithms]
│   │   ├── max_heap.hpp               [binary max heap]
│   │   ├── max_heap.cpp               [heapify up/down, extract max]
│   │   ├── lru_cache.hpp              [doubly linked list + hash map]
│   │   └── lru_cache.cpp              [get, put, eviction logic]
│   ├── network/                       [http server implementation]
│   │   ├── http_server.hpp            [main server class]
│   │   ├── http_server.cpp            [socket management, threading]
│   │   ├── request_parser.hpp         [parse http requests]
│   │   ├── request_parser.cpp         [header/body parsing]
│   │   ├── response_builder.hpp       [build http responses]
│   │   └── response_builder.cpp       [status codes, headers, body]
│   ├── api/                           [endpoint handlers]
│   │   ├── auth_handler.hpp           [authentication endpoints]
│   │   ├── auth_handler.cpp           [register, login, logout handlers]
│   │   ├── user_handler.hpp           [user management endpoints]
│   │   ├── user_handler.cpp           [search, profile, update handlers]
│   │   ├── friend_handler.hpp         [friend system endpoints]
│   │   ├── friend_handler.cpp         [request, accept, list handlers]
│   │   ├── match_handler.hpp          [match endpoints]
│   │   ├── match_handler.cpp          [queue, simulate, history handlers]
│   │   └── leaderboard_handler.hpp    [leaderboard endpoints]
│   │   └── leaderboard_handler.cpp    [ranking, stats handlers]
│   ├── services/                      [business logic layer]
│   │   ├── user_service.hpp           [user operations]
│   │   ├── user_service.cpp           [create, update, delete users]
│   │   ├── match_service.hpp          [match operations]
│   │   ├── match_service.cpp          [simulate, record, query matches]
│   │   ├── friend_service.hpp         [friend operations]
│   │   ├── friend_service.cpp         [add, remove, recommend friends]
│   │   └── session_service.hpp        [session operations]
│   │   └── session_service.cpp        [create, validate, expire sessions]
│   ├── models/                        [data structures]
│   │   ├── user.hpp                   [user struct definition]
│   │   ├── match.hpp                  [match struct definition]
│   │   ├── session.hpp                [session struct definition]
│   │   └── friend_request.hpp         [friend request struct]
│   ├── utils/                         [utility functions]
│   │   ├── password_hash.hpp          [hashing functions]
│   │   ├── password_hash.cpp          [sha256 + salt implementation]
│   │   ├── json_parser.hpp            [json utilities]
│   │   ├── json_parser.cpp            [parse/build json manually]
│   │   ├── file_utils.hpp             [file operations]
│   │   ├── file_utils.cpp             [read/write binary files]
│   │   ├── time_utils.hpp             [time utilities]
│   │   └── time_utils.cpp             [timestamp, formatting]
│   └── main.cpp                       [entry point]
└── data/                              [persistent storage]
    ├── users.dat                      [user database]
    ├── matches.dat                    [match history]
    ├── friends.dat                    [friend graph]
    └── sessions.dat                   [active sessions]
```

## core data structure specifications

### 1. hash_table.hpp/cpp

```cpp
template<typename K, typename V>
class hash_table {
private:
    struct node {
        K key;
        V value;
        node* next;
    };
    
    node** buckets;
    size_t capacity;
    size_t count;
    std::mutex mutex;
    
    size_t hash_function(const K& key);
    void resize();
    
public:
    hash_table(size_t initial_capacity = 1024);
    ~hash_table();
    
    bool insert(const K& key, const V& value);
    bool find(const K& key, V& value);
    bool remove(const K& key);
    bool update(const K& key, const V& value);
    size_t size();
    
    void serialize(const std::string& filename);
    void deserialize(const std::string& filename);
};
```

usage in project:
- user_database: hash_table<string, user_data>
- session_store: hash_table<string, session_data>
- username_to_id: hash_table<string, uint64_t>

### 2. b_tree.hpp/cpp

```cpp
template<typename K, typename V>
class b_tree {
private:
    static const int ORDER = 5;
    
    struct node {
        bool is_leaf;
        std::vector<K> keys;
        std::vector<V> values;
        std::vector<node*> children;
    };
    
    node* root;
    std::mutex mutex;
    
    void split_child(node* parent, int index);
    void merge_children(node* parent, int index);
    void insert_non_full(node* n, const K& key, const V& value);
    
public:
    b_tree();
    ~b_tree();
    
    bool insert(const K& key, const V& value);
    bool find(const K& key, V& value);
    std::vector<V> range_query(const K& start, const K& end);
    bool remove(const K& key);
    
    void serialize(const std::string& filename);
    void deserialize(const std::string& filename);
};
```

usage in project:
- match_history: b_tree<uint64_t, match_data> (timestamp-indexed)
- user_timeline: b_tree<uint64_t, timeline_event>

### 3. graph.hpp/cpp

```cpp
class graph {
private:
    std::unordered_map<uint64_t, std::vector<uint64_t>> adjacency_list;
    std::unordered_map<uint64_t, bool> online_status;
    std::mutex mutex;
    
public:
    graph();
    ~graph();
    
    bool add_vertex(uint64_t user_id);
    bool add_edge(uint64_t user1, uint64_t user2);
    bool remove_edge(uint64_t user1, uint64_t user2);
    
    std::vector<uint64_t> get_neighbors(uint64_t user_id);
    std::vector<uint64_t> bfs_recommendations(uint64_t user_id, int max_depth);
    std::vector<uint64_t> mutual_friends(uint64_t user1, uint64_t user2);
    
    bool is_connected(uint64_t user1, uint64_t user2);
    void set_online(uint64_t user_id, bool status);
    std::vector<uint64_t> get_online_friends(uint64_t user_id);
    
    void serialize(const std::string& filename);
    void deserialize(const std::string& filename);
};
```

usage in project:
- friend_graph: graph instance for all friendships

### 4. max_heap.hpp/cpp

```cpp
template<typename T>
class max_heap {
private:
    std::vector<T> heap;
    std::mutex mutex;
    
    void heapify_up(int index);
    void heapify_down(int index);
    int parent(int i);
    int left_child(int i);
    int right_child(int i);
    
public:
    max_heap();
    ~max_heap();
    
    void insert(const T& value);
    T extract_max();
    T peek_max();
    bool is_empty();
    size_t size();
    
    std::vector<T> get_top_n(int n);
};
```

usage in project:
- matchmaking_queue: max_heap<matchmaking_entry> (sorted by priority)
- leaderboard: max_heap<leaderboard_entry> (sorted by elo)

### 5. lru_cache.hpp/cpp

```cpp
template<typename K, typename V>
class lru_cache {
private:
    struct cache_node {
        K key;
        V value;
        cache_node* prev;
        cache_node* next;
    };
    
    size_t capacity;
    size_t count;
    cache_node* head;
    cache_node* tail;
    std::unordered_map<K, cache_node*> map;
    std::mutex mutex;
    
    void move_to_front(cache_node* node);
    void remove_node(cache_node* node);
    void add_to_front(cache_node* node);
    
public:
    lru_cache(size_t cap);
    ~lru_cache();
    
    bool get(const K& key, V& value);
    void put(const K& key, const V& value);
    bool exists(const K& key);
    void remove(const K& key);
    size_t size();
};
```

usage in project:
- session_cache: lru_cache<string, session_data> (capacity: 10000)
- user_cache: lru_cache<uint64_t, user_data> (capacity: 5000)

## model definitions

### user.hpp

```cpp
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
```

### match.hpp

```cpp
struct match_data {
    std::string match_id;
    uint64_t player1_id;
    uint64_t player2_id;
    std::string player1_username;
    std::string player2_username;
    int winner;
    int player1_elo_before;
    int player2_elo_before;
    int player1_elo_after;
    int player2_elo_after;
    uint64_t timestamp;
    int duration_seconds;
};

struct matchmaking_entry {
    uint64_t user_id;
    std::string username;
    int elo_rating;
    uint64_t queue_join_time;
    
    double get_priority() const {
        uint64_t wait_time = current_timestamp() - queue_join_time;
        return 0.6 * elo_rating + 0.4 * wait_time;
    }
    
    bool operator<(const matchmaking_entry& other) const {
        return get_priority() < other.get_priority();
    }
};
```

### session.hpp

```cpp
struct session_data {
    std::string session_token;
    uint64_t user_id;
    std::string username;
    uint64_t creation_timestamp;
    uint64_t expiration_timestamp;
    bool is_active;
};
```

## http server implementation

### http_server.hpp

```cpp
class http_server {
private:
    int server_socket;
    int port;
    bool running;
    std::vector<std::thread> worker_threads;
    
    static const int THREAD_POOL_SIZE = 10;
    static const int BACKLOG = 100;
    
    void worker_loop();
    void handle_connection(int client_socket);
    void route_request(const http_request& req, http_response& res);
    
public:
    http_server(int port);
    ~http_server();
    
    bool start();
    void stop();
    bool is_running();
};
```

### request_parser.hpp

```cpp
struct http_request {
    std::string method;
    std::string path;
    std::string query_string;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::unordered_map<std::string, std::string> query_params;
    std::unordered_map<std::string, std::string> path_params;
};

class request_parser {
public:
    static http_request parse(const std::string& raw_request);
    static std::unordered_map<std::string, std::string> parse_query_string(const std::string& query);
    static std::string url_decode(const std::string& str);
};
```

### response_builder.hpp

```cpp
struct http_response {
    int status_code;
    std::string status_message;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class response_builder {
public:
    static std::string build(const http_response& res);
    static http_response json_response(int status, const std::string& json);
    static http_response error_response(int status, const std::string& message);
    static http_response file_response(const std::string& filepath);
};
```

## api handler implementations

### auth_handler.cpp structure

```cpp
class auth_handler {
private:
    user_service& user_svc;
    session_service& session_svc;
    
public:
    http_response handle_register(const http_request& req);
    http_response handle_login(const http_request& req);
    http_response handle_logout(const http_request& req);
    http_response handle_session_check(const http_request& req);
};

http_response auth_handler::handle_register(const http_request& req) {
    parse json from req.body
    extract username, password
    
    validate username (3-20 chars, alphanumeric)
    validate password (8-128 chars)
    
    check if username exists in user_database
    if exists:
        return error_response(400, "username already taken")
    
    generate salt (32 bytes random)
    hash password with salt
    
    create user_data object
    assign unique user_id
    set initial elo = 1200
    set registration_timestamp
    
    insert into user_database hash_table
    add vertex to friend_graph
    
    return json_response(201, {success: true, user_id: ...})
}

http_response auth_handler::handle_login(const http_request& req) {
    parse json from req.body
    extract username, password
    
    find user in user_database hash_table
    if not found:
        return error_response(401, "invalid credentials")
    
    verify password hash
    if invalid:
        return error_response(401, "invalid credentials")
    
    check for existing active session
    if exists and not expired:
        return error_response(409, "already logged in")
    
    generate session_token (64 bytes random)
    create session_data
    set expiration = now + 24 hours
    
    insert into session_store hash_table
    add to session_cache lru_cache
    
    update user last_login_timestamp
    set user is_online = true
    
    return json_response(200, {token: session_token, user: ...})
}
```

### user_handler.cpp structure

```cpp
class user_handler {
private:
    user_service& user_svc;
    session_service& session_svc;
    
public:
    http_response handle_search(const http_request& req);
    http_response handle_get_profile(const http_request& req);
    http_response handle_update_profile(const http_request& req);
    http_response handle_upload_picture(const http_request& req);
};

http_response user_handler::handle_search(const http_request& req) {
    verify session token from headers
    
    extract query params:
        query (search string)
        sort (elo, username, join_date, win_rate)
        page (default 1)
        per_page (default 20)
    
    collect all users from user_database
    
    if query not empty:
        filter users where username contains query
    
    sort users based on sort parameter
    
    paginate results:
        start_index = (page - 1) * per_page
        end_index = start_index + per_page
    
    build json array of user previews
    
    return json_response(200, {users: [...], total: ..., page: ...})
}
```

### friend_handler.cpp structure

```cpp
class friend_handler {
private:
    friend_service& friend_svc;
    session_service& session_svc;
    
public:
    http_response handle_send_request(const http_request& req);
    http_response handle_accept_request(const http_request& req);
    http_response handle_list_friends(const http_request& req);
    http_response handle_recommendations(const http_request& req);
};

http_response friend_handler::handle_recommendations(const http_request& req) {
    verify session token
    get current user_id
    
    call friend_graph.bfs_recommendations(user_id, depth=2)
    
    filter out:
        - already friends
        - pending friend requests
        - self
    
    for each recommended user:
        calculate mutual friends count
        get user profile data
    
    sort by mutual friends count descending
    
    limit to top 20 recommendations
    
    return json_response(200, {recommendations: [...]})
}
```

### match_handler.cpp structure

```cpp
class match_handler {
private:
    match_service& match_svc;
    session_service& session_svc;
    
public:
    http_response handle_join_queue(const http_request& req);
    http_response handle_leave_queue(const http_request& req);
    http_response handle_queue_status(const http_request& req);
    http_response handle_simulate_match(const http_request& req);
    http_response handle_history(const http_request& req);
};

http_response match_handler::handle_simulate_match(const http_request& req) {
    verify session token
    get current user_id
    
    parse json body:
        opponent_username
        
    find opponent in user_database
    if not found:
        return error_response(404, "opponent not found")
    
    get both users' elo ratings
    
    simulate match outcome:
        random number 0-100
        win probability based on elo difference
        determine winner (0=draw, 1=player1, 2=player2)
    
    calculate elo changes:
        k_factor = 32
        expected_score = 1 / (1 + 10^((opponent_elo - player_elo)/400))
        actual_score = 1 if won, 0.5 if draw, 0 if lost
        elo_change = k_factor * (actual_score - expected_score)
    
    update both users' elo and stats
    
    create match_data object
    generate unique match_id
    set timestamp = now
    set duration = random(300, 1800) seconds
    
    insert into match_history b_tree
    
    return json_response(200, {match: {...}, elo_changes: {...}})
}
```

### leaderboard_handler.cpp structure

```cpp
class leaderboard_handler {
private:
    user_service& user_svc;
    
public:
    http_response handle_get_leaderboard(const http_request& req);
    http_response handle_get_rank(const http_request& req);
};

http_response leaderboard_handler::handle_get_leaderboard(const http_request& req) {
    extract query params:
        period (all, month, week)
        page (default 1)
        per_page (default 100)
    
    create max_heap of leaderboard_entry
    
    iterate all users from user_database:
        if period filtering required:
            check last_login_timestamp
        
        create leaderboard_entry with elo
        insert into max_heap
    
    extract top (page * per_page) entries
    
    calculate ranks
    
    slice for current page
    
    return json_response(200, {leaderboard: [...], total: ...})
}
```

## service layer implementations

### user_service.cpp

```cpp
class user_service {
private:
    hash_table<std::string, user_data>& user_db;
    graph& friend_graph;
    std::mutex& user_mutex;
    
public:
    bool create_user(const std::string& username, const std::string& password_hash, const std::string& salt);
    bool find_user(const std::string& username, user_data& user);
    bool update_user(const user_data& user);
    user_stats get_stats(uint64_t user_id);
    std::vector<user_data> search_users(const std::string& query, const std::string& sort_by);
};
```

### match_service.cpp

```cpp
class match_service {
private:
    b_tree<uint64_t, match_data>& match_history;
    hash_table<std::string, user_data>& user_db;
    max_heap<matchmaking_entry>& matchmaking_queue;
    std::mutex& match_mutex;
    
public:
    std::string simulate_match(uint64_t player1_id, uint64_t player2_id);
    std::vector<match_data> get_history(uint64_t user_id, int page, int per_page);
    std::vector<match_data> get_recent_matches(int count);
    match_data get_match(const std::string& match_id);
};
```

### friend_service.cpp

```cpp
class friend_service {
private:
    graph& friend_graph;
    hash_table<std::string, user_data>& user_db;
    std::mutex& friend_mutex;
    
public:
    bool add_friend(uint64_t user1_id, uint64_t user2_id);
    bool remove_friend(uint64_t user1_id, uint64_t user2_id);
    std::vector<uint64_t> get_friends(uint64_t user_id);
    std::vector<uint64_t> get_online_friends(uint64_t user_id);
    std::vector<uint64_t> get_recommendations(uint64_t user_id);
    std::vector<uint64_t> get_mutual_friends(uint64_t user1_id, uint64_t user2_id);
};
```

### session_service.cpp

```cpp
class session_service {
private:
    hash_table<std::string, session_data>& session_store;
    lru_cache<std::string, session_data>& session_cache;
    std::mutex& session_mutex;
    
public:
    std::string create_session(uint64_t user_id, const std::string& username);
    bool validate_session(const std::string& token, uint64_t& user_id);
    bool invalidate_session(const std::string& token);
    void cleanup_expired_sessions();
};
```

## utility implementations

### password_hash.cpp

```cpp
class password_utils {
public:
    static std::string generate_salt(size_t length = 32);
    static std::string hash_password(const std::string& password, const std::string& salt);
    static bool verify_password(const std::string& password, const std::string& hash, const std::string& salt);
    
private:
    static std::string sha256(const std::string& input);
    static std::string bytes_to_hex(const uint8_t* data, size_t length);
};
```

### json_parser.cpp

```cpp
class json_utils {
public:
    static std::string escape_string(const std::string& str);
    static std::string unescape_string(const std::string& str);
    
    static std::string build_object(const std::unordered_map<std::string, std::string>& fields);
    static std::string build_array(const std::vector<std::string>& items);
    
    static std::string extract_string(const std::string& json, const std::string& key);
    static int extract_int(const std::string& json, const std::string& key);
    static bool extract_bool(const std::string& json, const std::string& key);
};
```

## threading architecture

### main thread responsibilities
1. initialize all data structures
2. load persistent data
3. create http server
4. start worker threads
5. wait for shutdown signal
6. save persistent data
7. cleanup resources

### worker thread responsibilities
1. accept incoming connections
2. read http request
3. parse request
4. route to appropriate handler
5. execute handler logic
6. build http response
7. send response
8. close connection

### matchmaking thread responsibilities
1. run every 1 second
2. lock matchmaking_queue
3. extract top 2 entries
4. check elo difference < 200
5. if valid pair:
    - create match notification
    - store pending match
6. unlock queue
7. sleep 1 second

### persistence thread responsibilities
1. run every 30 seconds
2. lock all data structure mutexes
3. serialize user_database to users.dat
4. serialize match_history to matches.dat
5. serialize friend_graph to friends.dat
6. serialize active sessions to sessions.dat
7. unlock mutexes
8. sleep 30 seconds

### cleanup thread responsibilities
1. run every 5 minutes
2. lock session_store
3. iterate all sessions
4. remove expired sessions
5. unlock session_store
6. sleep 5 minutes

## mutex locking order

to prevent deadlocks, always acquire mutexes in this order:

1. user_mutex
2. session_mutex
3. friend_mutex
4. match_mutex
5. matchmaking_mutex

never acquire in reverse order
always release in reverse order of acquisition

## error handling strategy

### http error codes
- 200: success
- 201: created
- 400: bad request (invalid input)
- 401: unauthorized (invalid credentials)
- 403: forbidden (invalid session)
- 404: not found
- 409: conflict (duplicate, already logged in)
- 500: internal server error

### exception handling
- catch all exceptions in worker threads
- log error details
- return 500 response
- never crash server
- cleanup resources properly

### validation
- validate all inputs before processing
- check username format
- check password strength
- verify session tokens
- sanitize file uploads
- limit request sizes

## performance optimizations

### hash table
- initial capacity 1024
- load factor 0.75
- resize doubles capacity
- chaining for collision resolution

### b-tree
- order 5 (maximum 4 keys per node)
- cache frequently accessed nodes
- batch insertions when possible

### graph
- adjacency list representation
- cache bfs results for common queries
- lazy loading of neighbors

### max heap
- initial capacity 256
- dynamic resizing
- batch operations when possible

### lru cache
- capacity based on available memory
- efficient move-to-front operation
- periodic cleanup of stale entries

## memory management

### allocation strategy
- pre-allocate buffers for http requests/responses
- pool worker threads at startup
- reuse data structure nodes where possible
- limit cache sizes to prevent oom

### cleanup strategy
- close all file descriptors
- delete all dynamic allocations
- join all threads before exit
- flush pending writes to disk