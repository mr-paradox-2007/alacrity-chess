#include <iostream>
#include <thread>
#include <memory>
#include <csignal>
#include <sstream>

#include "server/src/network/http_server.hpp"
#include "server/src/api/game_state.hpp"
#include "server/src/utils/json_parser.hpp"

std::unique_ptr<game_state> game;
std::unique_ptr<http_server> server;

std::string handle_register(const http_request& req) {
    json_value body = json_parser::parse(req.body);
    
    if (body.value_type != json_value::object_type ||
        body.object_val.find("username") == body.object_val.end() ||
        body.object_val.find("password") == body.object_val.end()) {
        return "{\"error\":\"Invalid request\"}";
    }
    
    std::string username = body.object_val["username"].string_val;
    std::string password = body.object_val["password"].string_val;
    
    if (game->register_user(username, password)) {
        return "{\"status\":\"ok\",\"message\":\"User registered\"}";
    }
    
    return "{\"error\":\"Registration failed\"}";
}

std::string handle_login(const http_request& req) {
    json_value body = json_parser::parse(req.body);
    
    if (body.value_type != json_value::object_type ||
        body.object_val.find("username") == body.object_val.end() ||
        body.object_val.find("password") == body.object_val.end()) {
        return "{\"error\":\"Invalid request\"}";
    }
    
    std::string username = body.object_val["username"].string_val;
    std::string password = body.object_val["password"].string_val;
    std::string token;
    
    if (game->login_user(username, password, token)) {
        return "{\"status\":\"ok\",\"token\":\"" + token + "\"}";
    }
    
    return "{\"error\":\"Login failed\"}";
}

std::string handle_logout(const http_request& req) {
    auto it = req.headers.find("Authorization");
    if (it == req.headers.end()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    std::string token = it->second;
    if (game->logout_user(token)) {
        return "{\"status\":\"ok\"}";
    }
    
    return "{\"error\":\"Logout failed\"}";
}

std::string handle_get_user(const http_request& req) {
    auto it = req.headers.find("Authorization");
    if (it == req.headers.end()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(it->second, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    user_data user;
    if (game->get_user(user_id, user)) {
        return "{\"user_id\":" + std::to_string(user.user_id) + 
               ",\"username\":\"" + user.username + 
               "\",\"elo\":" + std::to_string(user.elo_rating) + "}";
    }
    
    return "{\"error\":\"User not found\"}";
}

std::string handle_leaderboard(const http_request& req) {
    return "{\"leaderboard\":[{\"rank\":1,\"username\":\"admin\",\"elo\":2000}]}";
}

std::string handle_get_friends(const http_request& req) {
    auto it = req.headers.find("Authorization");
    if (it == req.headers.end()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(it->second, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    std::vector<uint64_t> friends;
    game->get_friends(user_id, friends);
    return "{\"friends\":[]}";
}

std::string handle_health(const http_request& req) {
    return "{\"status\":\"ok\",\"message\":\"Chess Platform Server Running\"}";
}

int main() {
    game = std::make_unique<game_state>();
    server = std::make_unique<http_server>(3000);
    
    server->register_route("GET", "/health", handle_health);
    server->register_route("POST", "/auth/register", handle_register);
    server->register_route("POST", "/auth/login", handle_login);
    server->register_route("POST", "/auth/logout", handle_logout);
    server->register_route("GET", "/user/me", handle_get_user);
    server->register_route("GET", "/user/friends", handle_get_friends);
    server->register_route("GET", "/leaderboard", handle_leaderboard);
    
    std::cout << "Chess Platform Server starting on port 3000..." << std::endl;
    
    std::thread server_thread([&]() {
        server->start();
    });
    
    std::cout << "Server running. Press Ctrl+C to stop." << std::endl;
    std::signal(SIGINT, [](int sig) {
        std::cout << "\nShutting down..." << std::endl;
        exit(0);
    });
    
    server_thread.join();
    
    return 0;
}
