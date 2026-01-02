#include "http_server.hpp"
#include "globals.hpp"
#include <thread>
#include <sstream>

std::string HandleSignup(const std::string& body) {
    std::string username, password;
    
    size_t user_pos = body.find("\"username\":\"");
    if (user_pos != std::string::npos) {
        size_t user_end = body.find("\"", user_pos + 12);
        username = body.substr(user_pos + 12, user_end - user_pos - 12);
    }
    
    size_t pass_pos = body.find("\"password\":\"");
    if (pass_pos != std::string::npos) {
        size_t pass_end = body.find("\"", pass_pos + 12);
        password = body.substr(pass_pos + 12, pass_end - pass_pos - 12);
    }
    
    std::stringstream ss;
    if (g_user_manager.SignUp(username, password)) {
        g_logger.Info("Web signup: " + username);
        ss << "{\"success\":true,\"message\":\"Account created\"}";
    } else {
        g_logger.Warn("Web signup failed: " + username);
        ss << "{\"success\":false,\"error\":\"Username already exists\"}";
    }
    
    return ss.str();
}

std::string HandleLogin(const std::string& body) {
    std::string username, password;
    
    size_t user_pos = body.find("\"username\":\"");
    if (user_pos != std::string::npos) {
        size_t user_end = body.find("\"", user_pos + 12);
        username = body.substr(user_pos + 12, user_end - user_pos - 12);
    }
    
    size_t pass_pos = body.find("\"password\":\"");
    if (pass_pos != std::string::npos) {
        size_t pass_end = body.find("\"", pass_pos + 12);
        password = body.substr(pass_pos + 12, pass_end - pass_pos - 12);
    }
    
    std::stringstream ss;
    if (g_user_manager.Login(username, password)) {
        g_logger.Info("Web login: " + username);
        ss << "{\"success\":true,\"token\":\"token_" << username << "\"}";
    } else {
        g_logger.Warn("Web login failed: " + username);
        ss << "{\"success\":false,\"error\":\"Invalid credentials\"}";
    }
    
    return ss.str();
}
