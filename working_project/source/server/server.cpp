#include "server.hpp"
#include "globals.hpp"
#include "message_protocol.hpp"
#include <iostream>
#include <sstream>

ChessServer::ChessServer(int port) : server_socket(-1), port(port), running(false) {}

ChessServer::~ChessServer() {
    Stop();
}

bool ChessServer::Start() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        g_logger.Error("Server socket creation failed");
        return false;
    }
    
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        g_logger.Error("Setsockopt failed");
        return false;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        g_logger.Error("Bind failed");
        return false;
    }
    
    if (listen(server_socket, 5) < 0) {
        g_logger.Error("Listen failed");
        return false;
    }
    
    running = true;
    g_logger.Info("Chess server started on port " + std::to_string(port));
    return true;
}

void ChessServer::Stop() {
    running = false;
    if (server_socket >= 0) {
        close(server_socket);
        server_socket = -1;
    }
    
    std::unique_lock<std::mutex> lock(clients_mutex);
    for (auto& [fd, client] : clients) {
        close(fd);
    }
    clients.clear();
    g_logger.Info("Server stopped");
}

void ChessServer::HandleConnections() {
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            if (running) g_logger.Error("Accept failed");
            continue;
        }
        
        g_logger.Info("Client connected from " + std::string(inet_ntoa(client_addr.sin_addr)));
        
        ClientSession session;
        session.socket_fd = client_socket;
        session.authenticated = false;
        
        std::unique_lock<std::mutex> lock(clients_mutex);
        clients[client_socket] = session;
        lock.unlock();
        
        std::thread(&ChessServer::HandleClient, this, client_socket).detach();
    }
}

void ChessServer::HandleClient(int client_socket) {
    char buffer[4096] = {0};
    
    while (running) {
        int recv_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (recv_size < 0) {
            g_logger.Error("Receive error");
            break;
        }
        
        if (recv_size == 0) {
            g_logger.Info("Client disconnected");
            break;
        }
        
        buffer[recv_size] = '\0';
        Message msg = Message::FromJSON(buffer);
        
        std::string response_str;
        Message response;
        response.type = MSG_SUCCESS;
        
        switch (msg.type) {
            case MSG_SIGNUP: {
                std::string username = msg.data["username"];
                std::string password = msg.data["password"];
                
                if (g_user_manager.SignUp(username, password)) {
                    response.type = MSG_SUCCESS;
                    response.data["message"] = "Signup successful";
                    g_logger.Info("User signed up: " + username);
                } else {
                    response.type = MSG_ERROR;
                    response.data["error"] = "Username already exists";
                    g_logger.Warn("Signup failed for: " + username);
                }
                break;
            }
            
            case MSG_LOGIN: {
                std::string username = msg.data["username"];
                std::string password = msg.data["password"];
                
                if (g_user_manager.Login(username, password)) {
                    response.type = MSG_SUCCESS;
                    response.data["message"] = "Login successful";
                    
                    std::unique_lock<std::mutex> lock(clients_mutex);
                    if (clients.count(client_socket)) {
                        clients[client_socket].username = username;
                        clients[client_socket].authenticated = true;
                    }
                    lock.unlock();
                    
                    g_logger.Info("User logged in: " + username);
                } else {
                    response.type = MSG_ERROR;
                    response.data["error"] = "Invalid credentials";
                    g_logger.Warn("Login failed for: " + username);
                }
                break;
            }
            
            case MSG_QUEUE: {
                std::unique_lock<std::mutex> lock(clients_mutex);
                if (clients.count(client_socket) && clients[client_socket].authenticated) {
                    std::string username = clients[client_socket].username;
                    lock.unlock();
                    
                    g_matchmaking_queue.Enqueue(username, client_socket);
                    response.data["message"] = "Queued for matchmaking";
                    g_logger.Info("Player queued: " + username);
                    
                    if (g_matchmaking_queue.GetSize() >= 2) {
                        QueuedPlayer p1, p2;
                        g_matchmaking_queue.Dequeue(p1);
                        g_matchmaking_queue.Dequeue(p2);
                        g_logger.Info("Match created: " + p1.username + " vs " + p2.username);
                    }
                } else {
                    response.type = MSG_ERROR;
                    response.data["error"] = "Not authenticated";
                }
                break;
            }
            
            default:
                response.type = MSG_ERROR;
                response.data["error"] = "Unknown message type";
                break;
        }
        
        response_str = response.ToJSON();
        send(client_socket, response_str.c_str(), response_str.length(), 0);
    }
    
    {
        std::unique_lock<std::mutex> lock(clients_mutex);
        clients.erase(client_socket);
    }
    close(client_socket);
}
