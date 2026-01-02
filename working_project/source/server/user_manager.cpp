#include "user_manager.hpp"
#include <functional>
#include <chrono>

std::string UserManager::HashPassword(const std::string& password) const {
    std::hash<std::string> hasher;
    size_t hash_value = hasher(password);
    return std::to_string(hash_value);
}

bool UserManager::SignUp(const std::string& username, const std::string& password) {
    if (users.Contains(username)) {
        return false;
    }
    
    User new_user;
    new_user.username = username;
    new_user.password_hash = HashPassword(password);
    
    users.Insert(username, new_user);
    friends_graph.AddUser(username);
    return true;
}

bool UserManager::Login(const std::string& username, const std::string& password) {
    User user;
    if (!users.Find(username, user)) {
        return false;
    }
    
    return user.password_hash == HashPassword(password);
}

User* UserManager::GetUser(const std::string& username) {
    if (user_cache.Contains(username)) {
        return new User(user_cache.Get(username));
    }
    
    User user;
    if (users.Find(username, user)) {
        user_cache.Put(username, user);
        static User cached_user;
        cached_user = user;
        return &cached_user;
    }
    return nullptr;
}

bool UserManager::UserExists(const std::string& username) {
    return users.Contains(username);
}

void UserManager::UpdateUserStats(const std::string& username, bool won) {
    User user;
    if (users.Find(username, user)) {
        if (won) {
            user.wins++;
            user.rating += 10;
        } else {
            user.losses++;
            user.rating -= 5;
        }
        users.Insert(username, user);
        user_cache.Put(username, user);
    }
}

void UserManager::RecordGameHistory(const std::string& player1, const std::string& player2, 
                                     const std::string& winner, const std::string& moves) {
    long long timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    std::string record = player1 + " vs " + player2 + " | Winner: " + winner + " | Moves: " + moves;
    game_history.Insert(timestamp, record);
}

void UserManager::AddFriend(const std::string& user1, const std::string& user2) {
    friends_graph.AddFriendship(user1, user2);
}

void UserManager::RemoveFriend(const std::string& user1, const std::string& user2) {
    friends_graph.RemoveFriendship(user1, user2);
}

std::vector<std::string> UserManager::GetFriends(const std::string& username) {
    return friends_graph.GetFriends(username);
}
