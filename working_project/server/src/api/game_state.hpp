#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "../core/hash_table.hpp"
#include "../core/b_tree.hpp"
#include "../core/graph.hpp"
#include "../core/max_heap.hpp"
#include "../core/lru_cache.hpp"
#include "../models/user.hpp"
#include "../models/match.hpp"
#include "../models/session.hpp"
#include "../utils/password_hash.hpp"
#include "../utils/time_utils.hpp"
#include "../utils/file_utils.hpp"
#include "../utils/json_parser.hpp"
#include <mutex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <cstdlib>
#include <cctype>
#include <set>
#include <climits>

struct matchmaking_entry {
    uint64_t user_id;
    int elo_rating;
    uint64_t queue_time;
    
    bool operator<(const matchmaking_entry& other) const {
        double priority1 = 0.6 * elo_rating + 0.4 * (0);
        double priority2 = 0.6 * other.elo_rating + 0.4 * (0);
        return priority1 < priority2;
    }
    
    bool operator>(const matchmaking_entry& other) const {
        return other < *this;
    }
};

class game_state {
private:
    hash_table<std::string, user_data> users;
    hash_table<uint64_t, std::string> user_id_to_username;
    hash_table<std::string, session_data> sessions;
    b_tree<uint64_t, match_data> match_history;
    graph<std::string> friend_graph;
    max_heap<matchmaking_entry> matchmaking_queue;
    lru_cache<uint64_t, user_data> session_cache;
    hash_table<uint64_t, std::vector<uint64_t>> pending_friend_requests;
    
    uint64_t next_match_id;
    uint64_t next_user_id;
    
    std::mutex state_mutex;
    
    std::string get_username_by_id(uint64_t user_id);
    void save_users();
    void load_users();
    void save_friend_requests();
    void load_friend_requests();
    void save_match_history();
    void load_match_history();
    void save_friend_graph();
    void load_friend_graph();
    
public:
    game_state();
    
    bool register_user(const std::string& username, const std::string& password);
    bool login_user(const std::string& username, const std::string& password, std::string& token);
    bool logout_user(const std::string& token);
    bool verify_session(const std::string& token, uint64_t& user_id);
    
    uint64_t get_user_id_by_username(const std::string& username);
    
    bool get_user(uint64_t user_id, user_data& user);
    void get_all_users(std::vector<user_data>& users_list);
    bool update_user_elo(uint64_t user_id, int elo_change);
    
    bool send_friend_request(uint64_t sender_id, uint64_t receiver_id);
    bool accept_friend_request(uint64_t user_id, uint64_t friend_id);
    bool reject_friend_request(uint64_t user_id, uint64_t friend_id);
    void get_pending_friend_requests(uint64_t user_id, std::vector<uint64_t>& requests);
    void get_friends(uint64_t user_id, std::vector<uint64_t>& friends);
    void get_friend_recommendations(uint64_t user_id, std::vector<uint64_t>& recommendations);
    
    void queue_for_matchmaking(uint64_t user_id, int elo_rating);
    bool find_match(uint64_t user_id, uint64_t& opponent_id);
    
    bool record_match(uint64_t player1_id, uint64_t player2_id, uint64_t winner_id, int elo_change);
    void get_match_history(uint64_t user_id, std::vector<match_data>& history);
};

inline game_state::game_state() : users(2048), user_id_to_username(2048), sessions(1024), match_history(5), friend_graph(), session_cache(512), pending_friend_requests(1024) {
    next_match_id = 1;
    next_user_id = 1;
    try {
        load_users();
        load_friend_requests();
        load_match_history();
        load_friend_graph();
    } catch (...) {
        next_match_id = 1;
        next_user_id = 1;
    }
}

inline std::string game_state::get_username_by_id(uint64_t user_id) {
    std::string username;
    if (user_id_to_username.find(user_id, username)) {
        return username;
    }
    return "";
}

inline bool game_state::register_user(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    user_data existing;
    if (users.find(username, existing)) {
        return false;
    }
    
    user_data new_user;
    new_user.user_id = next_user_id++;
    new_user.username = username;
    new_user.salt = password_hash::generate_salt();
    new_user.password_hash = password_hash::hash_password(password, new_user.salt);
    new_user.elo_rating = 1600;
    new_user.total_matches = 0;
    new_user.wins = 0;
    new_user.losses = 0;
    new_user.draws = 0;
    new_user.registration_timestamp = time_utils::get_current_timestamp();
    new_user.last_login_timestamp = 0;
    new_user.is_online = false;
    
    users.insert(username, new_user);
    user_id_to_username.insert(new_user.user_id, username);
    friend_graph.add_vertex(new_user.user_id, username);
    save_users();
    
    return true;
}

inline bool game_state::login_user(const std::string& username, const std::string& password, std::string& token) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    user_data user;
    if (!users.find(username, user)) {
        return false;
    }
    
    if (!password_hash::verify_password(password, user.salt, user.password_hash)) {
        return false;
    }
    
    user.is_online = true;
    user.last_login_timestamp = time_utils::get_current_timestamp();
    users.update(username, user);
    friend_graph.set_online(user.user_id, true);
    save_users();
    
    session_data session;
    session.token = std::to_string(user.user_id) + "_" + std::to_string(time_utils::get_current_timestamp_ms());
    session.user_id = user.user_id;
    session.created_at = time_utils::get_current_timestamp();
    session.expires_at = session.created_at + 86400;
    session.is_valid = true;
    
    sessions.insert(session.token, session);
    session_cache.put(user.user_id, user);
    token = session.token;
    
    return true;
}

inline bool game_state::logout_user(const std::string& token) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    session_data session;
    if (!sessions.find(token, session)) {
        return false;
    }
    
    sessions.remove(token);
    
    std::string username = get_username_by_id(session.user_id);
    if (!username.empty()) {
        user_data user;
        if (users.find(username, user)) {
            user.is_online = false;
            users.update(username, user);
        }
    }
    
    friend_graph.set_online(session.user_id, false);
    
    return true;
}

inline bool game_state::verify_session(const std::string& token, uint64_t& user_id) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    session_data session;
    if (!sessions.find(token, session)) {
        return false;
    }
    
    if (!session.is_valid || session.expires_at < time_utils::get_current_timestamp()) {
        return false;
    }
    
    user_id = session.user_id;
    return true;
}

inline bool game_state::get_user(uint64_t user_id, user_data& user) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    if (session_cache.get(user_id, user)) {
        return true;
    }
    
    std::string username = get_username_by_id(user_id);
    if (!username.empty() && users.find(username, user)) {
        session_cache.put(user_id, user);
        return true;
    }
    
    return false;
}

inline void game_state::get_all_users(std::vector<user_data>& users_list) {
    std::lock_guard<std::mutex> lock(state_mutex);
    users_list.clear();
    users.iterate([&](const std::string& username, const user_data& user) {
        users_list.push_back(user);
    });
}

inline bool game_state::update_user_elo(uint64_t user_id, int elo_change) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    std::string username = get_username_by_id(user_id);
    if (username.empty()) {
        return false;
    }
    
    user_data user;
    if (!users.find(username, user)) {
        return false;
    }
    
    user.elo_rating += elo_change;
    users.update(username, user);
    session_cache.put(user_id, user);
    save_users();
    
    return true;
}

inline void game_state::queue_for_matchmaking(uint64_t user_id, int elo_rating) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    matchmaking_entry entry;
    entry.user_id = user_id;
    entry.elo_rating = elo_rating;
    entry.queue_time = time_utils::get_current_timestamp();
    
    matchmaking_queue.insert(entry);
}

inline bool game_state::find_match(uint64_t user_id, uint64_t& opponent_id) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    if (matchmaking_queue.empty() || matchmaking_queue.size() < 2) {
        return false;
    }
    
    try {
        std::vector<matchmaking_entry> candidates;
        matchmaking_entry user_entry;
        bool found_user = false;
        
        while (!matchmaking_queue.empty() && candidates.size() < 10) {
            matchmaking_entry entry = matchmaking_queue.extract_max();
            if (entry.user_id == user_id) {
                user_entry = entry;
                found_user = true;
                break;
            }
            candidates.push_back(entry);
        }
        
        if (!found_user) {
            for (const auto& e : candidates) {
                matchmaking_queue.insert(e);
            }
            return false;
        }
        
        if (matchmaking_queue.empty()) {
            matchmaking_queue.insert(user_entry);
            for (const auto& e : candidates) {
                matchmaking_queue.insert(e);
            }
            return false;
        }
        
        matchmaking_entry opponent_entry = matchmaking_queue.extract_max();
        opponent_id = opponent_entry.user_id;
        
        for (const auto& e : candidates) {
            matchmaking_queue.insert(e);
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

inline bool game_state::send_friend_request(uint64_t sender_id, uint64_t receiver_id) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    if (sender_id == receiver_id) {
        return false;
    }
    
    if (!friend_graph.contains_vertex(sender_id) || !friend_graph.contains_vertex(receiver_id)) {
        return false;
    }
    
    std::vector<uint64_t> current_friends;
    friend_graph.get_friends(sender_id, current_friends);
    if (std::find(current_friends.begin(), current_friends.end(), receiver_id) != current_friends.end()) {
        return false;
    }
    
    std::vector<uint64_t> pending;
    if (!pending_friend_requests.find(receiver_id, pending)) {
        pending = std::vector<uint64_t>();
    }
    
    if (std::find(pending.begin(), pending.end(), sender_id) != pending.end()) {
        return false;
    }
    
    pending.push_back(sender_id);
    pending_friend_requests.insert(receiver_id, pending);
    save_friend_requests();
    
    return true;
}

inline bool game_state::accept_friend_request(uint64_t user_id, uint64_t friend_id) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    if (!friend_graph.contains_vertex(user_id) || !friend_graph.contains_vertex(friend_id)) {
        return false;
    }
    
    std::vector<uint64_t> pending;
    if (!pending_friend_requests.find(user_id, pending)) {
        return false;
    }
    
    auto it = std::find(pending.begin(), pending.end(), friend_id);
    if (it == pending.end()) {
        return false;
    }
    
    pending.erase(it);
    if (pending.empty()) {
        pending_friend_requests.remove(user_id);
    } else {
        pending_friend_requests.insert(user_id, pending);
    }
    
    friend_graph.add_edge(user_id, friend_id);
    save_friend_requests();
    save_friend_graph();
    
    return true;
}

inline bool game_state::reject_friend_request(uint64_t user_id, uint64_t friend_id) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    std::vector<uint64_t> pending;
    if (!pending_friend_requests.find(user_id, pending)) {
        return false;
    }
    
    auto it = std::find(pending.begin(), pending.end(), friend_id);
    if (it == pending.end()) {
        return false;
    }
    
    pending.erase(it);
    if (pending.empty()) {
        pending_friend_requests.remove(user_id);
    } else {
        pending_friend_requests.insert(user_id, pending);
    }
    
    save_friend_requests();
    return true;
}

inline void game_state::get_pending_friend_requests(uint64_t user_id, std::vector<uint64_t>& requests) {
    std::lock_guard<std::mutex> lock(state_mutex);
    requests.clear();
    pending_friend_requests.find(user_id, requests);
}

inline void game_state::get_friends(uint64_t user_id, std::vector<uint64_t>& friends) {
    std::lock_guard<std::mutex> lock(state_mutex);
    friend_graph.get_friends(user_id, friends);
}

inline void game_state::get_friend_recommendations(uint64_t user_id, std::vector<uint64_t>& recommendations) {
    std::lock_guard<std::mutex> lock(state_mutex);
    friend_graph.get_friend_recommendations(user_id, 10, recommendations);
}

inline bool game_state::record_match(uint64_t player1_id, uint64_t player2_id, uint64_t winner_id, int elo_change) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    match_data match;
    match.match_id = next_match_id++;
    match.player1_id = player1_id;
    match.player2_id = player2_id;
    match.winner_id = winner_id;
    match.elo_change_p1 = elo_change;
    match.elo_change_p2 = -elo_change;
    match.timestamp = time_utils::get_current_timestamp();
    match.duration_seconds = 60 + (std::rand() % 300);
    match.result = (winner_id == player1_id ? 1 : 2);
    
    match_history.insert(match.timestamp, match);
    save_users();
    save_match_history();
    
    std::string username1 = get_username_by_id(player1_id);
    std::string username2 = get_username_by_id(player2_id);
    
    if (!username1.empty()) {
        user_data user;
        if (users.find(username1, user)) {
            user.elo_rating += elo_change;
            user.total_matches += 1;
            if (winner_id == player1_id) user.wins += 1;
            else user.losses += 1;
            users.update(username1, user);
            session_cache.put(player1_id, user);
        }
    }
    
    if (!username2.empty()) {
        user_data user;
        if (users.find(username2, user)) {
            user.elo_rating -= elo_change;
            user.total_matches += 1;
            if (winner_id == player2_id) user.wins += 1;
            else user.losses += 1;
            users.update(username2, user);
            session_cache.put(player2_id, user);
        }
    }
    
    return true;
}

inline void game_state::get_match_history(uint64_t user_id, std::vector<match_data>& history) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    uint64_t start_time = time_utils::get_current_timestamp() - (30 * 24 * 3600);
    uint64_t end_time = time_utils::get_current_timestamp();
    
    std::vector<std::pair<uint64_t, match_data>> results;
    match_history.range_query(start_time, end_time, results);
    
    for (const auto& [timestamp, match] : results) {
        if (match.player1_id == user_id || match.player2_id == user_id) {
            history.push_back(match);
        }
    }
}

inline uint64_t game_state::get_user_id_by_username(const std::string& username) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    user_data user;
    if (users.find(username, user)) {
        return user.user_id;
    }
    return 0;
}

inline void game_state::save_users() {
    std::string data_dir = "server/data";
    if (access("server/data", F_OK) != 0) {
        if (access("data", F_OK) == 0) {
            data_dir = "data";
        } else {
            system("mkdir -p server/data 2>/dev/null || mkdir -p data 2>/dev/null");
            if (access("server/data", F_OK) == 0) {
                data_dir = "server/data";
            } else if (access("data", F_OK) == 0) {
                data_dir = "data";
            } else {
                return;
            }
        }
    }
    
    std::string filename = data_dir + "/users.json";
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    auto escape_json = [](const std::string& str) -> std::string {
        std::string result;
        for (unsigned char c : str) {
            if (c == '"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else if (c == '\n') result += "\\n";
            else if (c == '\r') result += "\\r";
            else if (c == '\t') result += "\\t";
            else if (c < 32 || c > 126) {
                char hex[8];
                snprintf(hex, sizeof(hex), "\\u%04x", c);
                result += hex;
            } else {
                result += c;
            }
        }
        return result;
    };
    
    file << "{\"users\":[";
    bool first = true;
    users.iterate([&](const std::string& username, const user_data& user) {
        if (!first) file << ",";
        first = false;
        file << "{";
        file << "\"username\":\"" << escape_json(username) << "\",";
        file << "\"user_id\":" << user.user_id << ",";
        file << "\"password_hash\":\"" << escape_json(user.password_hash) << "\",";
        file << "\"salt\":\"" << escape_json(user.salt) << "\",";
        file << "\"elo_rating\":" << user.elo_rating << ",";
        file << "\"total_matches\":" << user.total_matches << ",";
        file << "\"wins\":" << user.wins << ",";
        file << "\"losses\":" << user.losses << ",";
        file << "\"draws\":" << user.draws << ",";
        file << "\"registration_timestamp\":" << user.registration_timestamp << ",";
        file << "\"last_login_timestamp\":" << user.last_login_timestamp << ",";
        file << "\"is_online\":" << (user.is_online ? "true" : "false");
        file << "}";
    });
    file << "],\"next_user_id\":" << next_user_id << ",\"next_match_id\":" << next_match_id << "}";
    file.close();
}

inline void game_state::load_users() {
    std::string filename = "server/data/users.json";
    if (access(filename.c_str(), F_OK) != 0) {
        filename = "data/users.json";
        if (access(filename.c_str(), F_OK) != 0) {
            return;
        }
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty() || content.length() < 10) {
        return;
    }
    
    try {
        json_value root = json_parser::parse(content);
        if (root.value_type != json_value::object_type) {
            return;
        }
        
        auto next_user_it = root.object_val.find("next_user_id");
        if (next_user_it != root.object_val.end() && next_user_it->second.value_type == json_value::number_type) {
            next_user_id = (uint64_t)next_user_it->second.number_val;
        }
        
        auto next_match_it = root.object_val.find("next_match_id");
        if (next_match_it != root.object_val.end() && next_match_it->second.value_type == json_value::number_type) {
            next_match_id = (uint64_t)next_match_it->second.number_val;
        }
        
        auto users_it = root.object_val.find("users");
        if (users_it != root.object_val.end() && users_it->second.value_type == json_value::array_type) {
            for (const auto& user_val : users_it->second.array_val) {
                if (user_val.value_type != json_value::object_type) continue;
                
                auto username_it = user_val.object_val.find("username");
                if (username_it == user_val.object_val.end() || username_it->second.value_type != json_value::string_type) {
                    continue;
                }
                
                std::string username = username_it->second.string_val;
                if (username.empty()) continue;
                
                auto user_id_it = user_val.object_val.find("user_id");
                auto password_hash_it = user_val.object_val.find("password_hash");
                auto salt_it = user_val.object_val.find("salt");
                
                if (user_id_it == user_val.object_val.end() || 
                    password_hash_it == user_val.object_val.end() ||
                    salt_it == user_val.object_val.end()) {
                    continue;
                }
                
                user_data user;
                user.user_id = (uint64_t)user_id_it->second.number_val;
                user.username = username;
                user.password_hash = password_hash_it->second.string_val;
                user.salt = salt_it->second.string_val;
                
                auto elo_it = user_val.object_val.find("elo_rating");
                user.elo_rating = (elo_it != user_val.object_val.end() && elo_it->second.value_type == json_value::number_type) ? (int)elo_it->second.number_val : 1600;
                
                auto matches_it = user_val.object_val.find("total_matches");
                user.total_matches = (matches_it != user_val.object_val.end() && matches_it->second.value_type == json_value::number_type) ? (int)matches_it->second.number_val : 0;
                
                auto wins_it = user_val.object_val.find("wins");
                user.wins = (wins_it != user_val.object_val.end() && wins_it->second.value_type == json_value::number_type) ? (int)wins_it->second.number_val : 0;
                
                auto losses_it = user_val.object_val.find("losses");
                user.losses = (losses_it != user_val.object_val.end() && losses_it->second.value_type == json_value::number_type) ? (int)losses_it->second.number_val : 0;
                
                auto draws_it = user_val.object_val.find("draws");
                user.draws = (draws_it != user_val.object_val.end() && draws_it->second.value_type == json_value::number_type) ? (int)draws_it->second.number_val : 0;
                
                auto reg_time_it = user_val.object_val.find("registration_timestamp");
                user.registration_timestamp = (reg_time_it != user_val.object_val.end() && reg_time_it->second.value_type == json_value::number_type) ? (uint64_t)reg_time_it->second.number_val : 0;
                
                auto login_time_it = user_val.object_val.find("last_login_timestamp");
                user.last_login_timestamp = (login_time_it != user_val.object_val.end() && login_time_it->second.value_type == json_value::number_type) ? (uint64_t)login_time_it->second.number_val : 0;
                
                auto online_it = user_val.object_val.find("is_online");
                user.is_online = (online_it != user_val.object_val.end() && online_it->second.value_type == json_value::bool_type) ? online_it->second.bool_val : false;
                
                users.insert(username, user);
                user_id_to_username.insert(user.user_id, username);
                friend_graph.add_vertex(user.user_id, username);
            }
        }
    } catch (...) {
    }
}

inline void game_state::save_friend_requests() {
    std::string data_dir = "server/data";
    if (access("server/data", F_OK) != 0) {
        if (access("data", F_OK) == 0) {
            data_dir = "data";
        } else {
            system("mkdir -p server/data 2>/dev/null || mkdir -p data 2>/dev/null");
            if (access("server/data", F_OK) == 0) {
                data_dir = "server/data";
            } else if (access("data", F_OK) == 0) {
                data_dir = "data";
            } else {
                return;
            }
        }
    }
    
    std::string filename = data_dir + "/friend_requests.json";
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\"requests\":[";
    bool first = true;
    pending_friend_requests.iterate([&](uint64_t receiver_id, const std::vector<uint64_t>& requests) {
        if (!first) file << ",";
        first = false;
        file << "{\"receiver_id\":" << receiver_id << ",\"senders\":[";
        for (size_t i = 0; i < requests.size(); i++) {
            if (i > 0) file << ",";
            file << requests[i];
        }
        file << "]}";
    });
    file << "]}";
    file.close();
}

inline void game_state::load_friend_requests() {
    std::string filename = "server/data/friend_requests.json";
    if (access(filename.c_str(), F_OK) != 0) {
        filename = "data/friend_requests.json";
        if (access(filename.c_str(), F_OK) != 0) {
            return;
        }
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty() || content.length() < 10) {
        return;
    }
    
    try {
        json_value root = json_parser::parse(content);
        if (root.value_type != json_value::object_type) {
            return;
        }
        
        auto requests_it = root.object_val.find("requests");
        if (requests_it != root.object_val.end() && requests_it->second.value_type == json_value::array_type) {
            for (const auto& req_val : requests_it->second.array_val) {
                if (req_val.value_type != json_value::object_type) continue;
                
                auto receiver_it = req_val.object_val.find("receiver_id");
                auto senders_it = req_val.object_val.find("senders");
                
                if (receiver_it == req_val.object_val.end() || 
                    senders_it == req_val.object_val.end() ||
                    senders_it->second.value_type != json_value::array_type) {
                    continue;
                }
                
                uint64_t receiver_id = (uint64_t)receiver_it->second.number_val;
                std::vector<uint64_t> requests;
                
                for (const auto& sender_val : senders_it->second.array_val) {
                    if (sender_val.value_type == json_value::number_type) {
                        requests.push_back((uint64_t)sender_val.number_val);
                    }
                }
                
                if (!requests.empty()) {
                    pending_friend_requests.insert(receiver_id, requests);
                }
            }
        }
    } catch (...) {
    }
}

inline void game_state::save_match_history() {
    std::string data_dir = "server/data";
    if (access("server/data", F_OK) != 0) {
        if (access("data", F_OK) == 0) {
            data_dir = "data";
        } else {
            system("mkdir -p server/data 2>/dev/null || mkdir -p data 2>/dev/null");
            if (access("server/data", F_OK) == 0) {
                data_dir = "server/data";
            } else if (access("data", F_OK) == 0) {
                data_dir = "data";
            } else {
                return;
            }
        }
    }
    
    std::string filename = data_dir + "/match_history.json";
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\"matches\":[";
    bool first = true;
    std::vector<std::pair<uint64_t, match_data>> all_matches;
    match_history.range_query(0, UINT64_MAX, all_matches);
    
    for (const auto& pair : all_matches) {
        const match_data& match = pair.second;
        if (!first) file << ",";
        first = false;
        file << "{";
        file << "\"match_id\":" << match.match_id << ",";
        file << "\"player1_id\":" << match.player1_id << ",";
        file << "\"player2_id\":" << match.player2_id << ",";
        file << "\"winner_id\":" << match.winner_id << ",";
        file << "\"elo_change_p1\":" << match.elo_change_p1 << ",";
        file << "\"elo_change_p2\":" << match.elo_change_p2 << ",";
        file << "\"timestamp\":" << match.timestamp << ",";
        file << "\"duration_seconds\":" << match.duration_seconds << ",";
        file << "\"result\":" << match.result;
        file << "}";
    }
    file << "]}";
    file.close();
}

inline void game_state::load_match_history() {
    std::string filename = "server/data/match_history.json";
    if (access(filename.c_str(), F_OK) != 0) {
        filename = "data/match_history.json";
        if (access(filename.c_str(), F_OK) != 0) {
            return;
        }
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty() || content.length() < 10) {
        return;
    }
    
    try {
        json_value root = json_parser::parse(content);
        if (root.value_type != json_value::object_type) {
            return;
        }
        
        auto matches_it = root.object_val.find("matches");
        if (matches_it != root.object_val.end() && matches_it->second.value_type == json_value::array_type) {
            for (const auto& match_val : matches_it->second.array_val) {
                if (match_val.value_type != json_value::object_type) continue;
                
                match_data match;
                auto match_id_it = match_val.object_val.find("match_id");
                auto p1_it = match_val.object_val.find("player1_id");
                auto p2_it = match_val.object_val.find("player2_id");
                auto winner_it = match_val.object_val.find("winner_id");
                auto timestamp_it = match_val.object_val.find("timestamp");
                
                if (match_id_it == match_val.object_val.end() ||
                    p1_it == match_val.object_val.end() ||
                    p2_it == match_val.object_val.end() ||
                    winner_it == match_val.object_val.end() ||
                    timestamp_it == match_val.object_val.end()) {
                    continue;
                }
                
                match.match_id = (uint64_t)match_id_it->second.number_val;
                match.player1_id = (uint64_t)p1_it->second.number_val;
                match.player2_id = (uint64_t)p2_it->second.number_val;
                match.winner_id = (uint64_t)winner_it->second.number_val;
                match.timestamp = (uint64_t)timestamp_it->second.number_val;
                
                auto elo1_it = match_val.object_val.find("elo_change_p1");
                match.elo_change_p1 = (elo1_it != match_val.object_val.end()) ? (int)elo1_it->second.number_val : 0;
                
                auto elo2_it = match_val.object_val.find("elo_change_p2");
                match.elo_change_p2 = (elo2_it != match_val.object_val.end()) ? (int)elo2_it->second.number_val : 0;
                
                auto duration_it = match_val.object_val.find("duration_seconds");
                match.duration_seconds = (duration_it != match_val.object_val.end()) ? (uint64_t)duration_it->second.number_val : 60;
                
                auto result_it = match_val.object_val.find("result");
                match.result = (result_it != match_val.object_val.end()) ? (int)result_it->second.number_val : 0;
                
                match_history.insert(match.timestamp, match);
                if (match.match_id >= next_match_id) {
                    next_match_id = match.match_id + 1;
                }
            }
        }
    } catch (...) {
    }
}

inline void game_state::save_friend_graph() {
    std::string data_dir = "server/data";
    if (access("server/data", F_OK) != 0) {
        if (access("data", F_OK) == 0) {
            data_dir = "data";
        } else {
            system("mkdir -p server/data 2>/dev/null || mkdir -p data 2>/dev/null");
            if (access("server/data", F_OK) == 0) {
                data_dir = "server/data";
            } else if (access("data", F_OK) == 0) {
                data_dir = "data";
            } else {
                return;
            }
        }
    }
    
    std::string filename = data_dir + "/friend_graph.json";
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\"edges\":[";
    bool first = true;
    std::set<std::pair<uint64_t, uint64_t>> edges_saved;
    
    for (uint64_t user_id = 1; user_id < next_user_id; user_id++) {
        if (!friend_graph.contains_vertex(user_id)) continue;
        
        std::vector<uint64_t> friends;
        friend_graph.get_friends(user_id, friends);
        
        for (uint64_t friend_id : friends) {
            if (user_id < friend_id) {
                if (!first) file << ",";
                first = false;
                file << "{\"user1\":" << user_id << ",\"user2\":" << friend_id << "}";
                edges_saved.insert({user_id, friend_id});
            }
        }
    }
    
    file << "]}";
    file.close();
}

inline void game_state::load_friend_graph() {
    std::string filename = "server/data/friend_graph.json";
    if (access(filename.c_str(), F_OK) != 0) {
        filename = "data/friend_graph.json";
        if (access(filename.c_str(), F_OK) != 0) {
            return;
        }
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty() || content.length() < 10) {
        return;
    }
    
    try {
        json_value root = json_parser::parse(content);
        if (root.value_type != json_value::object_type) {
            return;
        }
        
        auto edges_it = root.object_val.find("edges");
        if (edges_it != root.object_val.end() && edges_it->second.value_type == json_value::array_type) {
            for (const auto& edge_val : edges_it->second.array_val) {
                if (edge_val.value_type != json_value::object_type) continue;
                
                auto user1_it = edge_val.object_val.find("user1");
                auto user2_it = edge_val.object_val.find("user2");
                
                if (user1_it == edge_val.object_val.end() ||
                    user2_it == edge_val.object_val.end()) {
                    continue;
                }
                
                uint64_t user1 = (uint64_t)user1_it->second.number_val;
                uint64_t user2 = (uint64_t)user2_it->second.number_val;
                
                if (friend_graph.contains_vertex(user1) && friend_graph.contains_vertex(user2)) {
                    friend_graph.add_edge(user1, user2);
                }
            }
        }
    } catch (...) {
    }
}

#endif
