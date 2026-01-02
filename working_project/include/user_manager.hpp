#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include <string>
#include <cstring>
#include "hash_table.hpp"
#include "b_tree.hpp"
#include "lru_cache.hpp"
#include "graph.hpp"

struct User {
    std::string username;
    std::string password_hash;
    int wins;
    int losses;
    int rating;
    std::string friends;
    
    User() : wins(0), losses(0), rating(1000), friends("") {}
};

class UserManager {
private:
    HashTable<std::string, User> users;
    BTree<long long, std::string> game_history;
    LRUCache<std::string, User> user_cache;
    FriendsGraph friends_graph;
    
    std::string HashPassword(const std::string& password) const;
    
public:
    UserManager() : user_cache(100) {}
    
    bool SignUp(const std::string& username, const std::string& password);
    bool Login(const std::string& username, const std::string& password);
    User* GetUser(const std::string& username);
    bool UserExists(const std::string& username);
    void UpdateUserStats(const std::string& username, bool won);
    void RecordGameHistory(const std::string& player1, const std::string& player2, const std::string& winner, const std::string& moves);
    
    void AddFriend(const std::string& user1, const std::string& user2);
    void RemoveFriend(const std::string& user1, const std::string& user2);
    std::vector<std::string> GetFriends(const std::string& username);
    FriendsGraph& GetFriendsGraph() { return friends_graph; }
};

#endif
