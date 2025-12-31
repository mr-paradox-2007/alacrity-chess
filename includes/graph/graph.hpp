#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>

namespace ds
{

class graph
{
private:
    // Adjacency list to store the graph
    std::unordered_map<int, std::vector<int>> adj_list_;

    // Mutex for thread safety
    mutable std::mutex lock_;

public:
    graph();
    ~graph();

    // Adds a bidirectional edge between two players.
    // Returns true on success, false if they are already friends.
    bool add_friend(int player_id_a, int player_id_b);

    // Removes the bidirectional edge between two players.
    // Returns true on success, false if they were not friends.
    bool remove_friend(int player_id_a, int player_id_b);

    // Checks if two players are friends.
    bool are_friends(int player_id_a, int player_id_b) const;

    // Returns a vector of PlayerIDs for a given player's friends.
    // Returns an empty vector if the player has no friends or does not exist.
    std::vector<int> get_friends(int player_id) const;

    // Returns a vector of recommended friends by finding friends of friends (2-hops away).
    // This MUST be implemented using a Breadth-First Search (BFS) algorithm.
    // The result should not include the original player or their direct friends.
    std::vector<int> get_friends_of_friends(int player_id) const;

    // Persists the graph's adjacency list to a binary file.
    void serialize_to_disk(const std::string& filename) const;

    // Loads the graph's adjacency list from a binary file.
    // Should clear the existing graph before loading. Returns true on success.
    bool deserialize_from_disk(const std::string& filename);

    // Returns the total number of players (nodes) in the graph.
    size_t size() const;

    // Deletes all data from the graph.
    void clear();

    // Disable copy constructor and assignment operator
    graph(const graph&) = delete;
    graph& operator=(const graph&) = delete;
};

} // namespace ds

#endif // GRAPH_HPP
