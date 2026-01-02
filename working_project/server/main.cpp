#include <iostream>
#include <thread>
#include <memory>
#include <csignal>
#include <sstream>
#include <unistd.h>
#include <limits.h>
#include <algorithm>
#include <vector>
#include <sys/stat.h>

#include "src/network/http_server.hpp"
#include "src/api/game_state.hpp"
#include "src/utils/json_parser.hpp"

std::unique_ptr<game_state> game;
std::unique_ptr<http_server> server;

std::string extract_token(const http_request& req);
std::string handle_get_pending_friend_requests(const http_request& req);
std::string handle_get_friends(const http_request& req);
std::string handle_reject_friend_request(const http_request& req);

std::string handle_register(const http_request& req) {
    json_value body = json_parser::parse(req.body);
    
    if (body.value_type != json_value::object_type ||
        body.object_val.find("username") == body.object_val.end() ||
        body.object_val.find("password") == body.object_val.end()) {
        return "{\"error\":\"Invalid request\"}";
    }
    
    std::string username = body.object_val["username"].string_val;
    std::string password = body.object_val["password"].string_val;
    std::string token;
    
    if (game->register_user(username, password)) {
        if (game->login_user(username, password, token)) {
            int user_id = game->get_user_id_by_username(username);
            return "{\"status\":\"ok\",\"message\":\"User registered\",\"token\":\"" + token + "\",\"user_id\":" + std::to_string(user_id) + "}";
        }
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

std::string extract_token(const http_request& req) {
    auto it = req.headers.find("Authorization");
    if (it == req.headers.end()) {
        return "";
    }
    std::string auth_header = it->second;
    if (auth_header.find("Bearer ") == 0) {
        return auth_header.substr(7);
    }
    return auth_header;
}

std::string handle_logout(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    if (game->logout_user(token)) {
        return "{\"status\":\"ok\"}";
    }
    
    return "{\"error\":\"Logout failed\"}";
}

std::string handle_get_user(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    user_data user;
    if (game->get_user(user_id, user)) {
        std::string json = "{\"user_id\":" + std::to_string(user.user_id) + 
               ",\"username\":\"" + user.username + 
               "\",\"elo\":" + std::to_string(user.elo_rating) + 
               ",\"matches\":" + std::to_string(user.total_matches) +
               ",\"wins\":" + std::to_string(user.wins) +
               ",\"losses\":" + std::to_string(user.losses) +
               ",\"draws\":" + std::to_string(user.draws) + "}";
        return json;
    }
    
    return "{\"error\":\"User not found\"}";
}

std::string handle_leaderboard(const http_request& req) {
    // Get all users and sort by Elo rating (using max heap approach)
    std::vector<std::pair<int, user_data>> all_users;
    
    // Use the game_state's internal method to get all users
    // We'll need to add a method to game_state to get all users
    // For now, let's use a workaround by accessing through user_id_to_username
    std::vector<user_data> users_list;
    game->get_all_users(users_list);
    
    // Sort by Elo rating (descending)
    std::sort(users_list.begin(), users_list.end(), 
        [](const user_data& a, const user_data& b) {
            return a.elo_rating > b.elo_rating;
        });
    
    std::string result = "{\"leaderboard\":[";
    bool first = true;
    int rank = 1;
    
    // Return top 50 players
    for (size_t i = 0; i < users_list.size() && i < 50; i++) {
        const user_data& user = users_list[i];
        if (!first) result += ",";
        result += "{\"rank\":" + std::to_string(rank) + 
                  ",\"user_id\":" + std::to_string(user.user_id) +
                  ",\"username\":\"" + user.username + 
                  "\",\"elo\":" + std::to_string(user.elo_rating) +
                  ",\"matches\":" + std::to_string(user.total_matches) +
                  ",\"wins\":" + std::to_string(user.wins) +
                  ",\"losses\":" + std::to_string(user.losses) + "}";
        first = false;
        rank++;
    }
    
    result += "]}";
    return result;
}

std::string handle_search_users(const http_request& req) {
    std::string query_str = req.path;
    size_t q_pos = query_str.find("q=");
    if (q_pos == std::string::npos) {
        return "{\"users\":[]}";
    }
    
    std::string query = query_str.substr(q_pos + 2);
    if (query.length() > 50) query = query.substr(0, 50);
    
    std::string result = "{\"users\":[";
    bool first = true;
    
    for (int i = 1; i <= 50; i++) {
        user_data user;
        if (game->get_user(i, user)) {
            if (user.username.find(query) != std::string::npos) {
                if (!first) result += ",";
                result += "{\"user_id\":" + std::to_string(user.user_id) +
                          ",\"username\":\"" + user.username +
                          "\",\"elo\":" + std::to_string(user.elo_rating) + "}";
                first = false;
            }
        }
    }
    
    result += "]}";
    return result;
}

std::string handle_queue_for_match(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    user_data user;
    if (!game->get_user(user_id, user)) {
        return "{\"error\":\"User not found\"}";
    }
    
    game->queue_for_matchmaking(user_id, user.elo_rating);
    return "{\"status\":\"ok\",\"message\":\"Queued for matchmaking\"}";
}

std::string handle_find_match(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    uint64_t opponent_id = 0;
    if (game->find_match(user_id, opponent_id)) {
        user_data opponent;
        if (game->get_user(opponent_id, opponent)) {
            return "{\"status\":\"ok\",\"opponent_id\":" + std::to_string(opponent_id) +
                   ",\"opponent_username\":\"" + opponent.username + "\"}";
        }
    }
    
    return "{\"status\":\"waiting\"}";
}

std::string handle_get_match_history(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    std::vector<match_data> history;
    game->get_match_history(user_id, history);
    
    std::string result = "{\"matches\":[";
    bool first = true;
    for (const auto& match : history) {
        if (!first) result += ",";
        int change = (match.player1_id == user_id) ? match.elo_change_p1 : match.elo_change_p2;
        uint64_t opponent_id = (match.player1_id == user_id) ? match.player2_id : match.player1_id;
        
        user_data opponent;
        std::string opponent_name = "Unknown";
        if (game->get_user(opponent_id, opponent)) {
            opponent_name = opponent.username;
        }
        
        result += "{\"match_id\":" + std::to_string(match.match_id) +
                  ",\"player1_id\":" + std::to_string(match.player1_id) +
                  ",\"player2_id\":" + std::to_string(match.player2_id) +
                  ",\"opponent_id\":" + std::to_string(opponent_id) +
                  ",\"opponent_username\":\"" + opponent_name + "\"" +
                  ",\"winner_id\":" + std::to_string(match.winner_id) +
                  ",\"elo_change\":" + std::to_string(change) + "}";
        first = false;
    }
    result += "]}";
    return result;
}

std::string handle_record_match(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    json_value body = json_parser::parse(req.body);
    if (body.value_type != json_value::object_type ||
        body.object_val.find("opponent_id") == body.object_val.end() ||
        body.object_val.find("winner_id") == body.object_val.end()) {
        return "{\"error\":\"Invalid request\"}";
    }
    
    uint64_t opponent_id = (uint64_t)body.object_val["opponent_id"].number_val;
    uint64_t winner_id = (uint64_t)body.object_val["winner_id"].number_val;
    
    // Handle draw (winner_id == 0 means draw)
    int elo_change = 0;
    if (winner_id == 0) {
        // Draw - no Elo change
        elo_change = 0;
    } else if (winner_id == user_id) {
        elo_change = 16;  // Win
    } else {
        elo_change = -16;  // Loss
    }
    
    if (game->record_match(user_id, opponent_id, winner_id, elo_change)) {
        return "{\"status\":\"ok\",\"elo_change\":" + std::to_string(elo_change) + "}";
    }
    
    return "{\"error\":\"Failed to record match\"}";
}

std::string handle_send_friend_request(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    json_value body = json_parser::parse(req.body);
    if (body.value_type != json_value::object_type ||
        body.object_val.find("friend_id") == body.object_val.end()) {
        return "{\"error\":\"Invalid request\"}";
    }
    
    uint64_t friend_id = (uint64_t)body.object_val["friend_id"].number_val;
    
    if (game->send_friend_request(user_id, friend_id)) {
        return "{\"status\":\"ok\",\"message\":\"Friend request sent\"}";
    }
    
    return "{\"error\":\"Failed to send friend request\"}";
}

std::string handle_accept_friend_request(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    json_value body = json_parser::parse(req.body);
    if (body.value_type != json_value::object_type ||
        body.object_val.find("friend_id") == body.object_val.end()) {
        return "{\"error\":\"Invalid request\"}";
    }
    
    uint64_t friend_id = (uint64_t)body.object_val["friend_id"].number_val;
    
    if (game->accept_friend_request(user_id, friend_id)) {
        return "{\"status\":\"ok\",\"message\":\"Friend request accepted\"}";
    }
    
    return "{\"error\":\"Failed to accept friend request\"}";
}

std::string handle_get_pending_friend_requests(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    std::vector<uint64_t> pending;
    game->get_pending_friend_requests(user_id, pending);
    
    std::string result = "{\"requests\":[";
    bool first = true;
    for (uint64_t sender_id : pending) {
        user_data sender;
        if (game->get_user(sender_id, sender)) {
            if (!first) result += ",";
            result += "{\"user_id\":" + std::to_string(sender_id) +
                      ",\"username\":\"" + sender.username +
                      "\",\"elo\":" + std::to_string(sender.elo_rating) + "}";
            first = false;
        }
    }
    result += "]}";
    return result;
}

std::string handle_get_friends(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    std::vector<uint64_t> friends;
    game->get_friends(user_id, friends);
    
    std::string result = "{\"friends\":[";
    bool first = true;
    for (uint64_t friend_id : friends) {
        user_data friend_user;
        if (game->get_user(friend_id, friend_user)) {
            if (!first) result += ",";
            result += "{\"user_id\":" + std::to_string(friend_id) +
                      ",\"username\":\"" + friend_user.username +
                      "\",\"elo\":" + std::to_string(friend_user.elo_rating) + "}";
            first = false;
        }
    }
    result += "]}";
    return result;
}

std::string handle_reject_friend_request(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    json_value body = json_parser::parse(req.body);
    if (body.value_type != json_value::object_type ||
        body.object_val.find("friend_id") == body.object_val.end()) {
        return "{\"error\":\"Invalid request\"}";
    }
    
    uint64_t friend_id = (uint64_t)body.object_val["friend_id"].number_val;
    
    if (game->reject_friend_request(user_id, friend_id)) {
        return "{\"status\":\"ok\",\"message\":\"Friend request rejected\"}";
    }
    
    return "{\"error\":\"Failed to reject friend request\"}";
}

std::string handle_friend_recommendations(const http_request& req) {
    std::string token = extract_token(req);
    if (token.empty()) {
        return "{\"error\":\"Missing token\"}";
    }
    
    uint64_t user_id = 0;
    if (!game->verify_session(token, user_id)) {
        return "{\"error\":\"Invalid session\"}";
    }
    
    std::vector<uint64_t> recommendations;
    game->get_friend_recommendations(user_id, recommendations);
    
    std::string result = "{\"recommendations\":[";
    bool first = true;
    for (uint64_t rec_id : recommendations) {
        user_data rec_user;
        if (game->get_user(rec_id, rec_user)) {
            if (!first) result += ",";
            result += "{\"user_id\":" + std::to_string(rec_id) +
                      ",\"username\":\"" + rec_user.username +
                      "\",\"elo\":" + std::to_string(rec_user.elo_rating) + "}";
            first = false;
        }
    }
    result += "]}";
    return result;
}

std::string handle_health(const http_request& req) {
    return "{\"status\":\"ok\",\"message\":\"Chess Platform Server Running\"}";
}

int main() {
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        std::string exe_dir = std::string(exe_path);
        size_t last_slash = exe_dir.find_last_of("/");
        if (last_slash != std::string::npos) {
            exe_dir = exe_dir.substr(0, last_slash);
            if (exe_dir.find("/bin") != std::string::npos) {
                size_t bin_pos = exe_dir.find("/bin");
                if (bin_pos != std::string::npos) {
                    exe_dir = exe_dir.substr(0, bin_pos);
                } else {
                    exe_dir = exe_dir.substr(0, exe_dir.find_last_of("/"));
                }
            }
            chdir(exe_dir.c_str());
        }
    }
    
    std::string client_path = "client";
    if (access("client", F_OK) != 0) {
        if (access("../client", F_OK) == 0) {
            client_path = "../client";
        } else if (access("../../client", F_OK) == 0) {
            client_path = "../../client";
        }
    }
    
    game = std::make_unique<game_state>();
    server = std::make_unique<http_server>(8080, client_path);
    
    server->register_route("GET", "/health", handle_health);
    server->register_route("POST", "/auth/register", handle_register);
    server->register_route("POST", "/auth/login", handle_login);
    server->register_route("POST", "/auth/logout", handle_logout);
    server->register_route("GET", "/user/me", handle_get_user);
    server->register_route("GET", "/leaderboard", handle_leaderboard);
    server->register_route("GET", "/users/search", handle_search_users);
    server->register_route("POST", "/match/queue", handle_queue_for_match);
    server->register_route("POST", "/match/find", handle_find_match);
    server->register_route("GET", "/match/history", handle_get_match_history);
    server->register_route("POST", "/match/record", handle_record_match);
    server->register_route("POST", "/friends/request", handle_send_friend_request);
    server->register_route("POST", "/friends/accept", handle_accept_friend_request);
    server->register_route("POST", "/friends/reject", handle_reject_friend_request);
    server->register_route("GET", "/friends/pending", handle_get_pending_friend_requests);
    server->register_route("GET", "/friends/list", handle_get_friends);
    server->register_route("GET", "/friends/recommendations", handle_friend_recommendations);
    
    std::cout << "Chess Platform Server starting on port 8080..." << std::endl;
    
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
