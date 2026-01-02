#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <queue>
#include <set>
#include <mutex>
#include <cstdint>
#include <algorithm>

template<typename V>
class graph {
private:
    struct vertex_info {
        V data;
        bool is_online;
        std::set<uint64_t> neighbors;
    };
    
    std::unordered_map<uint64_t, vertex_info> vertices;
    mutable std::mutex mutex_lock;
    
    void bfs_helper(uint64_t start, int depth, std::set<uint64_t>& visited,
                   std::set<uint64_t>& friends_of_friends) const;
    
public:
    graph() = default;
    
    void add_vertex(uint64_t id, const V& data);
    void remove_vertex(uint64_t id);
    void add_edge(uint64_t u, uint64_t v);
    void remove_edge(uint64_t u, uint64_t v);
    
    bool contains_vertex(uint64_t id) const;
    V get_vertex(uint64_t id) const;
    bool update_vertex(uint64_t id, const V& data);
    
    void set_online(uint64_t id, bool online);
    bool is_online(uint64_t id) const;
    
    void get_friends(uint64_t id, std::vector<uint64_t>& friends) const;
    void get_online_friends(uint64_t id, std::vector<uint64_t>& friends) const;
    void get_mutual_friends(uint64_t id1, uint64_t id2,
                           std::vector<uint64_t>& mutual) const;
    void get_friend_recommendations(uint64_t id, int count,
                                   std::vector<uint64_t>& recommendations) const;
    
    void clear();
};

template<typename V>
void graph<V>::add_vertex(uint64_t id, const V& data) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    if (vertices.find(id) == vertices.end()) {
        vertices[id] = {data, false, {}};
    }
}

template<typename V>
void graph<V>::remove_vertex(uint64_t id) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it = vertices.find(id);
    if (it != vertices.end()) {
        for (auto& neighbor_id : it->second.neighbors) {
            auto neighbor_it = vertices.find(neighbor_id);
            if (neighbor_it != vertices.end()) {
                neighbor_it->second.neighbors.erase(id);
            }
        }
        vertices.erase(it);
    }
}

template<typename V>
void graph<V>::add_edge(uint64_t u, uint64_t v) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    if (vertices.find(u) != vertices.end() && vertices.find(v) != vertices.end()) {
        if (u != v) {
            vertices[u].neighbors.insert(v);
            vertices[v].neighbors.insert(u);
        }
    }
}

template<typename V>
void graph<V>::remove_edge(uint64_t u, uint64_t v) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    if (vertices.find(u) != vertices.end() && vertices.find(v) != vertices.end()) {
        vertices[u].neighbors.erase(v);
        vertices[v].neighbors.erase(u);
    }
}

template<typename V>
bool graph<V>::contains_vertex(uint64_t id) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    return vertices.find(id) != vertices.end();
}

template<typename V>
V graph<V>::get_vertex(uint64_t id) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it = vertices.find(id);
    if (it != vertices.end()) {
        return it->second.data;
    }
    throw std::runtime_error("Vertex not found");
}

template<typename V>
bool graph<V>::update_vertex(uint64_t id, const V& data) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it = vertices.find(id);
    if (it != vertices.end()) {
        it->second.data = data;
        return true;
    }
    return false;
}

template<typename V>
void graph<V>::set_online(uint64_t id, bool online) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it = vertices.find(id);
    if (it != vertices.end()) {
        it->second.is_online = online;
    }
}

template<typename V>
bool graph<V>::is_online(uint64_t id) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it = vertices.find(id);
    if (it != vertices.end()) {
        return it->second.is_online;
    }
    return false;
}

template<typename V>
void graph<V>::get_friends(uint64_t id, std::vector<uint64_t>& friends) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it = vertices.find(id);
    if (it != vertices.end()) {
        friends.assign(it->second.neighbors.begin(), it->second.neighbors.end());
    }
}

template<typename V>
void graph<V>::get_online_friends(uint64_t id, std::vector<uint64_t>& friends) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it = vertices.find(id);
    if (it != vertices.end()) {
        for (auto neighbor_id : it->second.neighbors) {
            auto neighbor_it = vertices.find(neighbor_id);
            if (neighbor_it != vertices.end() && neighbor_it->second.is_online) {
                friends.push_back(neighbor_id);
            }
        }
    }
}

template<typename V>
void graph<V>::get_mutual_friends(uint64_t id1, uint64_t id2,
                                  std::vector<uint64_t>& mutual) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it1 = vertices.find(id1);
    auto it2 = vertices.find(id2);
    if (it1 != vertices.end() && it2 != vertices.end()) {
        const auto& friends1 = it1->second.neighbors;
        const auto& friends2 = it2->second.neighbors;
        for (auto friend_id : friends1) {
            if (friends2.find(friend_id) != friends2.end()) {
                mutual.push_back(friend_id);
            }
        }
    }
}

template<typename V>
void graph<V>::bfs_helper(uint64_t start, int depth, std::set<uint64_t>& visited,
                         std::set<uint64_t>& friends_of_friends) const {
    if (depth == 0) return;
    std::queue<std::pair<uint64_t, int>> q;
    q.push({start, 0});
    visited.insert(start);
    while (!q.empty()) {
        auto [node, d] = q.front();
        q.pop();
        if (d < depth) {
            auto it = vertices.find(node);
            if (it != vertices.end()) {
                for (auto neighbor : it->second.neighbors) {
                    if (visited.find(neighbor) == visited.end()) {
                        visited.insert(neighbor);
                        if (d + 1 == depth) {
                            friends_of_friends.insert(neighbor);
                        } else if (d + 1 < depth) {
                            q.push({neighbor, d + 1});
                        }
                    }
                }
            }
        }
    }
}

template<typename V>
void graph<V>::get_friend_recommendations(uint64_t id, int count,
                                         std::vector<uint64_t>& recommendations) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    std::set<uint64_t> visited;
    std::set<uint64_t> fof;
    bfs_helper(id, 2, visited, fof);
    auto it = vertices.find(id);
    if (it != vertices.end()) {
        for (auto friend_id : it->second.neighbors) {
            fof.erase(friend_id);
        }
        fof.erase(id);
    }
    int i = 0;
    for (auto rec_id : fof) {
        if (i >= count) break;
        recommendations.push_back(rec_id);
        i++;
    }
}

template<typename V>
void graph<V>::clear() {
    std::lock_guard<std::mutex> lock(mutex_lock);
    vertices.clear();
}

#endif
