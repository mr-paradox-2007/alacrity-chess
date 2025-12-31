#include <graph/graph.hpp>
#include <algorithm>
#include <fstream>
#include <queue>

namespace ds
{

graph::graph()
{
}

graph::~graph()
{
    clear();
}

bool graph::add_friend(int player_id_a, int player_id_b)
{
    std::lock_guard<std::mutex> lock(lock_);

    if(player_id_a == player_id_b)
    {
        return false;
    }

    // Check if already friends
    auto& friends_a = adj_list_[player_id_a];
    if(std::find(friends_a.begin(), friends_a.end(), player_id_b) != friends_a.end())
    {
        return false;
    }

    // Add bidirectional edge
    adj_list_[player_id_a].push_back(player_id_b);
    adj_list_[player_id_b].push_back(player_id_a);

    return true;
}

bool graph::remove_friend(int player_id_a, int player_id_b)
{
    std::lock_guard<std::mutex> lock(lock_);

    // Check if both players exist
    if(adj_list_.find(player_id_a) == adj_list_.end() || adj_list_.find(player_id_b) == adj_list_.end())
    {
        return false;
    }

    // Find and remove from player_a's friend list
    auto& friends_a = adj_list_[player_id_a];
    auto it_a = std::find(friends_a.begin(), friends_a.end(), player_id_b);
    if(it_a == friends_a.end())
    {
        return false;
    }

    friends_a.erase(it_a);

    // Remove from player_b's friend list
    auto& friends_b = adj_list_[player_id_b];
    auto it_b = std::find(friends_b.begin(), friends_b.end(), player_id_a);
    if(it_b != friends_b.end())
    {
        friends_b.erase(it_b);
    }

    return true;
}

bool graph::are_friends(int player_id_a, int player_id_b) const
{
    std::lock_guard<std::mutex> lock(lock_);

    auto it = adj_list_.find(player_id_a);
    if(it == adj_list_.end())
    {
        return false;
    }

    const auto& friends = it->second;
    return std::find(friends.begin(), friends.end(), player_id_b) != friends.end();
}

std::vector<int> graph::get_friends(int player_id) const
{
    std::lock_guard<std::mutex> lock(lock_);

    auto it = adj_list_.find(player_id);
    if(it == adj_list_.end())
    {
        return std::vector<int>();
    }

    return it->second;
}

std::vector<int> graph::get_friends_of_friends(int player_id) const
{
    std::lock_guard<std::mutex> lock(lock_);

    std::vector<int> result;

    // Check if player exists
    auto it = adj_list_.find(player_id);
    if(it == adj_list_.end())
    {
        return result;
    }

    // Get direct friends
    const auto& direct_friends = it->second;

    // Create a set to track visited nodes and direct friends
    std::unordered_map<int, bool> visited;
    visited[player_id] = true;
    for(int friend_id : direct_friends)
    {
        visited[friend_id] = true;
    }

    // BFS to find friends of friends
    std::queue<int> queue;

    // Start BFS from direct friends
    for(int friend_id : direct_friends)
    {
        queue.push(friend_id);
    }

    while(!queue.empty())
    {
        int current = queue.front();
        queue.pop();

        // Get friends of current node
        auto friends_it = adj_list_.find(current);
        if(friends_it != adj_list_.end())
        {
            for(int friend_of_friend : friends_it->second)
            {
                if(!visited[friend_of_friend])
                {
                    visited[friend_of_friend] = true;
                    result.push_back(friend_of_friend);
                }
            }
        }
    }

    return result;
}

void graph::serialize_to_disk(const std::string& filename) const
{
    std::lock_guard<std::mutex> lock(lock_);

    std::ofstream file(filename, std::ios::binary);
    if(!file.is_open())
    {
        return;
    }

    // Write number of nodes
    size_t num_nodes = adj_list_.size();
    file.write(reinterpret_cast<const char*>(&num_nodes), sizeof(num_nodes));

    // Write each node and its friends
    for(const auto& pair : adj_list_)
    {
        int player_id = pair.first;
        const auto& friends = pair.second;

        file.write(reinterpret_cast<const char*>(&player_id), sizeof(player_id));

        size_t num_friends = friends.size();
        file.write(reinterpret_cast<const char*>(&num_friends), sizeof(num_friends));

        for(int friend_id : friends)
        {
            file.write(reinterpret_cast<const char*>(&friend_id), sizeof(friend_id));
        }
    }

    file.close();
}

bool graph::deserialize_from_disk(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(lock_);

    std::ifstream file(filename, std::ios::binary);
    if(!file.is_open())
    {
        return false;
    }

    // Clear existing data
    adj_list_.clear();

    // Read number of nodes
    size_t num_nodes;
    file.read(reinterpret_cast<char*>(&num_nodes), sizeof(num_nodes));

    // Read each node and its friends
    for(size_t i = 0; i < num_nodes; ++i)
    {
        int player_id;
        file.read(reinterpret_cast<char*>(&player_id), sizeof(player_id));

        size_t num_friends;
        file.read(reinterpret_cast<char*>(&num_friends), sizeof(num_friends));

        std::vector<int> friends;
        for(size_t j = 0; j < num_friends; ++j)
        {
            int friend_id;
            file.read(reinterpret_cast<char*>(&friend_id), sizeof(friend_id));
            friends.push_back(friend_id);
        }

        adj_list_[player_id] = friends;
    }

    file.close();
    return true;
}

size_t graph::size() const
{
    std::lock_guard<std::mutex> lock(lock_);
    return adj_list_.size();
}

void graph::clear()
{
    std::lock_guard<std::mutex> lock(lock_);
    adj_list_.clear();
}

} // namespace ds
