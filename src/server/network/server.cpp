#include "server/server.hpp"
#include "../../includes/server/auth_api.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <map>

namespace ac {
namespace server {

// Global response map for synchronous responses
std::map<int, std::string> response_map;
std::mutex response_mutex;

Server::Server() {
    std::cout << "[SERVER] Initializing Alacrity Chess Server (Simplified)..." << std::endl;
    
    // Initialize all 5 data structures (optional for Phase 1)
    try {
        hash_table_ = std::make_unique<ds::hash_table>();
        b_tree_ = std::make_unique<ds::b_tree>();
        graph_ = std::make_unique<ds::graph>();
        max_heap_ = std::make_unique<ds::max_heap>();
        lru_cache_ = std::make_unique<ds::lru_cache<std::string, std::string>>(1000);
        
        std::cout << "[SERVER] ✓ All 5 data structures initialized" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[SERVER] ⚠ Data structures init skipped: " << e.what() << std::endl;
    }
    
    // Initialize Auth API with hash_table for user storage
    try {
        auth_api_ = std::make_unique<api::AuthAPI>(hash_table_.get());
        std::cout << "[SERVER] ✓ Auth API initialized" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[SERVER] Error initializing Auth API: " << e.what() << std::endl;
    }
    
    std::cout << "[SERVER] ✓ Server initialization complete" << std::endl;
}

Server::~Server() {
    std::cout << "[SERVER] Shutting down server..." << std::endl;
    stop();
}

void Server::run(int port) {
    running_ = true;
    
    std::cout << "[SERVER] Starting server on port " << port << std::endl;
    
    // Start worker thread
    worker_thread_ = std::thread([this]() { worker_loop(); });
    std::cout << "[SERVER] ✓ Worker thread started" << std::endl;
    
    // Start listener thread
    listener_thread_ = std::thread([this, port]() { listener_loop(port); });
    std::cout << "[SERVER] ✓ Listener thread started" << std::endl;
    
    // Wait for threads to finish
    if (listener_thread_.joinable()) listener_thread_.join();
    if (worker_thread_.joinable()) worker_thread_.join();
    
    std::cout << "[SERVER] ✓ Server shutdown complete" << std::endl;
}

void Server::stop() {
    std::cout << "[SERVER] Stopping server..." << std::endl;
    running_ = false;
    
    if (server_socket_ >= 0) {
        close(server_socket_);
        server_socket_ = -1;
    }
}

void Server::listener_loop(int port) {
    std::cout << "[LISTENER] Starting TCP listener on port " << port << std::endl;
    
    // Create socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        std::cerr << "[LISTENER] Error: Could not create socket" << std::endl;
        running_ = false;
        return;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "[LISTENER] Error: setsockopt failed" << std::endl;
        close(server_socket_);
        running_ = false;
        return;
    }
    
    // Bind socket
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(server_socket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[LISTENER] Error: Could not bind to port " << port << std::endl;
        close(server_socket_);
        running_ = false;
        return;
    }
    
    // Listen
    if (listen(server_socket_, 5) < 0) {
        std::cerr << "[LISTENER] Error: Could not listen" << std::endl;
        close(server_socket_);
        running_ = false;
        return;
    }
    
    std::cout << "[LISTENER] ✓ Server listening on 127.0.0.1:" << port << std::endl;
    std::cout << "[LISTENER] Ready to accept client connections" << std::endl;
    
    int client_count = 0;
    
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            if (running_) {
                std::cerr << "[LISTENER] Error: Could not accept connection" << std::endl;
            }
            continue;
        }
        
        client_count++;
        std::cout << "[LISTENER] Client #" << client_count << " connected from " 
                  << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
        
        // Read some data from client
        char buffer[512] = {0};
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read > 0) {
            std::string message(buffer);
            std::cout << "[LISTENER] Received (" << bytes_read << " bytes): " << message.substr(0, 80) << std::endl;
            
            // Process immediately in listener thread for synchronous API
            if (auth_api_) {
                std::string response = auth_api_->handle_request(message);
                std::cout << "[LISTENER] Sending response: " << response.substr(0, 80) << std::endl;
                
                // Send response back to client
                send(client_socket, response.c_str(), response.length(), 0);
            }
        }
        
        close(client_socket);
    }
    
    close(server_socket_);
    std::cout << "[LISTENER] Listener thread ended" << std::endl;
}

void Server::worker_loop() {
    std::cout << "[WORKER] Worker thread loop started" << std::endl;
    std::cout << "[WORKER] Processing messages from FIFO queue (for async operations)" << std::endl;
    
    int processed_count = 0;
    
    while (running_) {
        Message msg;
        bool has_message = false;
        
        // Dequeue one message with lock
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (!message_queue_.empty()) {
                msg = message_queue_.front();
                message_queue_.pop();
                has_message = true;
            }
        }
        
        if (has_message) {
            processed_count++;
            std::string preview = msg.content.substr(0, 60);
            if (msg.content.length() > 60) preview += "...";
            std::cout << "[WORKER] Processing queued message #" << processed_count << " from client " 
                      << msg.client_id << ": " << preview << std::endl;
            
            // Process authentication requests
            if (auth_api_) {
                std::string response = auth_api_->handle_request(msg.content);
                std::cout << "[WORKER] Generated response: " << response.substr(0, 80) << std::endl;
                
                // TODO: Send response back to client in Phase 1.5
                // For now, we log the response
            }
            
        } else {
            // Brief sleep to prevent busy-waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    std::cout << "[WORKER] Worker thread loop ended (processed " << processed_count << " messages)" << std::endl;
}

}  // namespace server
}  // namespace ac
