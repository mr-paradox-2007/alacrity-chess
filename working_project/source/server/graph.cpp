#include "graph.hpp"
#include <algorithm>

void FriendsGraph::AddUser(const std::string& username) {
    if (nodes.find(username) == nodes.end()) {
        nodes[username] = Node(username);
    }
}

void FriendsGraph::AddFriendship(const std::string& user1, const std::string& user2) {
    AddUser(user1);
    AddUser(user2);
    
    auto& friends1 = nodes[user1].friends;
    if (std::find(friends1.begin(), friends1.end(), user2) == friends1.end()) {
        friends1.push_back(user2);
    }
    
    auto& friends2 = nodes[user2].friends;
    if (std::find(friends2.begin(), friends2.end(), user1) == friends2.end()) {
        friends2.push_back(user1);
    }
}

void FriendsGraph::RemoveFriendship(const std::string& user1, const std::string& user2) {
    if (nodes.find(user1) != nodes.end()) {
        auto& friends1 = nodes[user1].friends;
        friends1.erase(std::remove(friends1.begin(), friends1.end(), user2), friends1.end());
    }
    
    if (nodes.find(user2) != nodes.end()) {
        auto& friends2 = nodes[user2].friends;
        friends2.erase(std::remove(friends2.begin(), friends2.end(), user1), friends2.end());
    }
}

std::vector<std::string> FriendsGraph::GetFriends(const std::string& username) {
    if (nodes.find(username) != nodes.end()) {
        return nodes[username].friends;
    }
    return std::vector<std::string>();
}

std::vector<std::string> FriendsGraph::GetOnlineFriends(const std::string& username) {
    std::vector<std::string> online_friends;
    if (nodes.find(username) != nodes.end()) {
        for (const auto& friend_name : nodes[username].friends) {
            if (nodes[friend_name].online) {
                online_friends.push_back(friend_name);
            }
        }
    }
    return online_friends;
}

void FriendsGraph::SetUserOnline(const std::string& username, bool online) {
    if (nodes.find(username) != nodes.end()) {
        nodes[username].online = online;
    }
}

bool FriendsGraph::AreFriends(const std::string& user1, const std::string& user2) {
    if (nodes.find(user1) != nodes.end()) {
        auto& friends = nodes[user1].friends;
        return std::find(friends.begin(), friends.end(), user2) != friends.end();
    }
    return false;
}

std::vector<std::string> FriendsGraph::GetMutualFriends(const std::string& user1, const std::string& user2) {
    std::vector<std::string> mutual;
    auto friends1 = GetFriends(user1);
    auto friends2 = GetFriends(user2);
    
    for (const auto& f : friends1) {
        if (std::find(friends2.begin(), friends2.end(), f) != friends2.end()) {
            mutual.push_back(f);
        }
    }
    return mutual;
}
