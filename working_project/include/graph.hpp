#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <string>
#include <queue>

class FriendsGraph {
private:
    struct Node {
        std::string username;
        std::vector<std::string> friends;
        bool online;
        
        Node(const std::string& name = "") : username(name), online(false) {}
    };
    
    std::unordered_map<std::string, Node> nodes;
    
public:
    void AddUser(const std::string& username);
    void AddFriendship(const std::string& user1, const std::string& user2);
    void RemoveFriendship(const std::string& user1, const std::string& user2);
    std::vector<std::string> GetFriends(const std::string& username);
    std::vector<std::string> GetOnlineFriends(const std::string& username);
    void SetUserOnline(const std::string& username, bool online);
    bool AreFriends(const std::string& user1, const std::string& user2);
    std::vector<std::string> GetMutualFriends(const std::string& user1, const std::string& user2);
};

#endif
