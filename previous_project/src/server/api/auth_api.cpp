#include "../../includes/server/auth_api.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>

namespace ac {
namespace server {
namespace api {

AuthAPI::AuthAPI(ds::hash_table* user_table) : user_table_(user_table) {
    std::cout << "[AUTH_API] Initialized with user_table" << std::endl;
}

std::string AuthAPI::handle_request(const std::string& raw_message) {
    // Parse JSON request
    // Expected format: {"action":"register","username":"user1","password":"pass123"}
    
    std::string action = extract_field(raw_message, "action");
    std::string username = extract_field(raw_message, "username");
    std::string password = extract_field(raw_message, "password");
    
    std::cout << "[AUTH_API] Request: action=" << action << " username=" << username << std::endl;
    
    if (action == "register") {
        return handle_register(username, password);
    } else if (action == "login") {
        return handle_login(username, password);
    } else {
        return "{\"status\":\"error\",\"message\":\"Unknown action: " + action + "\"}";
    }
}

std::string AuthAPI::handle_register(const std::string& username, const std::string& password) {
    std::cout << "[AUTH_API] Processing REGISTER for: " << username << std::endl;
    
    // Validate credentials
    if (!is_valid_credentials(username, password)) {
        return "{\"status\":\"error\",\"message\":\"Invalid username or password format\"}";
    }
    
    // Create new user with user_info structure
    ds::user_info new_user;
    new_user.password_hash = password;  // TODO: proper hashing in Phase 2
    new_user.elo_rating = 1200;
    new_user.is_online = false;
    new_user.last_active = std::time(nullptr);
    
    // Generate token (simple: username + timestamp)
    auto now = std::time(nullptr);
    std::ostringstream token_stream;
    token_stream << username << "_" << now;
    new_user.session_token = token_stream.str();
    
    // Insert into hash table (will fail if user exists)
    if (user_table_->insert(username, new_user)) {
        std::cout << "[AUTH_API] ✓ User registered: " << username << std::endl;
        
        return "{\"status\":\"success\",\"message\":\"Registration successful\",\"token\":\"" + 
               new_user.session_token + "\"}";
    } else {
        std::cerr << "[AUTH_API] Failed to register (user may already exist): " << username << std::endl;
        return "{\"status\":\"error\",\"message\":\"Registration failed - user may already exist\"}";
    }
}

std::string AuthAPI::handle_login(const std::string& username, const std::string& password) {
    std::cout << "[AUTH_API] Processing LOGIN for: " << username << std::endl;
    
    if (username.empty() || password.empty()) {
        return "{\"status\":\"error\",\"message\":\"Username and password required\"}";
    }
    
    // Use authenticate method from hash_table
    if (!user_table_->authenticate(username, password)) {
        std::cout << "[AUTH_API] Authentication failed: " << username << std::endl;
        return "{\"status\":\"error\",\"message\":\"Invalid username or password\"}";
    }
    
    // Create new session token
    auto now = std::time(nullptr);
    std::ostringstream token_stream;
    token_stream << username << "_" << now << "_" << "active";
    std::string token = token_stream.str();
    
    std::cout << "[AUTH_API] ✓ User logged in: " << username << " (ELO: 1200)" << std::endl;
    
    std::ostringstream response;
    response << "{\"status\":\"success\",\"message\":\"Login successful\","
             << "\"token\":\"" << token << "\","
             << "\"username\":\"" << username << "\","
             << "\"elo_rating\":1200}";
    
    return response.str();
}

std::string AuthAPI::extract_field(const std::string& json, const std::string& field) {
    // Simple JSON field extraction
    // Look for "field":"value" pattern
    
    std::string search_pattern = "\"" + field + "\":\"";
    size_t start = json.find(search_pattern);
    
    if (start == std::string::npos) {
        return "";
    }
    
    start += search_pattern.length();
    size_t end = json.find("\"", start);
    
    if (end == std::string::npos) {
        return "";
    }
    
    return json.substr(start, end - start);
}

bool AuthAPI::is_valid_credentials(const std::string& username, const std::string& password) {
    // Basic validation
    if (username.empty() || password.empty()) return false;
    if (username.length() < 3 || username.length() > 20) return false;
    if (password.length() < 4 || password.length() > 128) return false;
    
    // Username should be alphanumeric (+ underscore)
    for (char c : username) {
        if (!std::isalnum(c) && c != '_') return false;
    }
    
    return true;
}

}  // namespace api
}  // namespace server
}  // namespace ac
