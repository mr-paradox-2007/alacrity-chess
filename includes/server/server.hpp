#pragma once

#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <string>
#include <iostream>
#include <cstdint>

// Include all 5 data structures
#include "hash_table/hash_table.hpp"
#include "b_tree/b_tree.hpp"
#include "graph/graph.hpp"
#include "heap/heap.hpp"
#include "lru_cache/lru_cache.hpp"

namespace ac {
namespace server {

// Forward declaration
namespace api {
class AuthAPI;
}

// Simple message struct for FIFO queue
struct Message {
    std::string content;
    int client_id = -1;
};

class Server {
public:
    // Lifecycle
    Server();
    ~Server();
    
    void run(int port);
    void stop();
    
    // Data structure accessors
    ds::hash_table* get_hash_table() { return hash_table_.get(); }
    ds::b_tree* get_b_tree() { return b_tree_.get(); }
    ds::graph* get_graph() { return graph_.get(); }
    ds::max_heap* get_max_heap() { return max_heap_.get(); }
    ds::lru_cache<std::string, std::string>* get_lru_cache() { return lru_cache_.get(); }
    
    // Queue accessors for message processing
    std::queue<Message>& get_message_queue() { return message_queue_; }
    std::mutex& get_queue_mutex() { return queue_mutex_; }
    
    // Status
    bool is_running() const { return running_; }

private:
    // Data structures (will be initialized when Phase 1 uses them)
    std::unique_ptr<ds::hash_table> hash_table_;
    std::unique_ptr<ds::b_tree> b_tree_;
    std::unique_ptr<ds::graph> graph_;
    std::unique_ptr<ds::max_heap> max_heap_;
    std::unique_ptr<ds::lru_cache<std::string, std::string>> lru_cache_;
    
    // API Handlers
    std::unique_ptr<api::AuthAPI> auth_api_;
    
    // Message queue and synchronization
    std::queue<Message> message_queue_;
    std::mutex queue_mutex_;
    
    // Worker thread for processing messages
    std::thread worker_thread_;
    std::thread listener_thread_;
    bool running_ = false;
    int server_socket_ = -1;
    
    // Worker loop (runs in separate thread)
    void worker_loop();
    void listener_loop(int port);
};

}  // namespace server
}  // namespace ac
