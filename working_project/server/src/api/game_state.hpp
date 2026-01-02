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
#include <mutex>

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
    
    uint64_t next_match_id;
    uint64_t next_user_id;
    
    std::mutex state_mutex;
    
    std::string get_username_by_id(uint64_t user_id);
    
public:
    game_state();
    
    bool register_user(const std::string& username, const std::string& password);
    bool login_user(const std::string& username, const std::string& password, std::string& token);
    bool logout_user(const std::string& token);
    bool verify_session(const std::string& token, uint64_t& user_id);
    
    uint64_t get_user_id_by_username(const std::string& username);
    
    bool get_user(uint64_t user_id, user_data& user);
    bool update_user_elo(uint64_t user_id, int elo_change);
    
    bool send_friend_request(uint64_t sender_id, uint64_t receiver_id);
    bool accept_friend_request(uint64_t user_id, uint64_t friend_id);
    void get_friends(uint64_t user_id, std::vector<uint64_t>& friends);
    void get_friend_recommendations(uint64_t user_id, std::vector<uint64_t>& recommendations);
    
    void queue_for_matchmaking(uint64_t user_id, int elo_rating);
    bool find_match(uint64_t user_id, uint64_t& opponent_id);
    
    bool record_match(uint64_t player1_id, uint64_t player2_id, uint64_t winner_id, int elo_change);
    void get_match_history(uint64_t user_id, std::vector<match_data>& history);
};

inline game_state::game_state() : users(2048), user_id_to_username(2048), sessions(1024), match_history(5), friend_graph(), session_cache(512) {
    next_match_id = 1;
    next_user_id = 1;
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
    
    if (!friend_graph.contains_vertex(sender_id) || !friend_graph.contains_vertex(receiver_id)) {
        return false;
    }
    
    friend_graph.add_edge(sender_id, receiver_id);
    return true;
}

inline bool game_state::accept_friend_request(uint64_t user_id, uint64_t friend_id) {
    std::lock_guard<std::mutex> lock(state_mutex);
    
    if (!friend_graph.contains_vertex(user_id) || !friend_graph.contains_vertex(friend_id)) {
        return false;
    }
    
    friend_graph.add_edge(user_id, friend_id);
    return true;
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
    
    update_user_elo(player1_id, elo_change);
    update_user_elo(player2_id, -elo_change);
    
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

#endif
