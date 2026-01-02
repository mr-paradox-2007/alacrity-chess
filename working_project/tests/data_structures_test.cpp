#include <iostream>
#include <string>
#include <cassert>
#include <cstdint>

#include "../server/src/core/hash_table.hpp"
#include "../server/src/core/b_tree.hpp"
#include "../server/src/core/graph.hpp"
#include "../server/src/core/max_heap.hpp"
#include "../server/src/core/lru_cache.hpp"

void test_hash_table() {
    std::cout << "Testing hash_table..." << std::endl;
    
    hash_table<std::string, int> ht(16);
    
    assert(ht.empty());
    assert(ht.size() == 0);
    
    for (int i = 0; i < 100; i++) {
        std::string key = "key_" + std::to_string(i);
        assert(ht.insert(key, i));
    }
    
    assert(ht.size() == 100);
    assert(!ht.empty());
    
    int value;
    assert(ht.find("key_50", value));
    assert(value == 50);
    
    assert(ht.contains("key_99"));
    assert(!ht.contains("key_999"));
    
    assert(ht.update("key_50", 500));
    assert(ht.find("key_50", value));
    assert(value == 500);
    
    assert(ht.remove("key_50"));
    assert(!ht.contains("key_50"));
    assert(ht.size() == 99);
    
    ht.clear();
    assert(ht.empty());
    
    std::cout << "hash_table tests passed!" << std::endl;
}

void test_b_tree() {
    std::cout << "Testing b_tree..." << std::endl;
    
    b_tree<int, std::string> bt(5);
    
    for (int i = 0; i < 50; i++) {
        bt.insert(i, "value_" + std::to_string(i));
    }
    
    std::string value;
    assert(bt.find(25, value));
    assert(value == "value_25");
    
    std::vector<std::pair<int, std::string>> results;
    bt.range_query(10, 20, results);
    assert(results.size() >= 10);
    
    assert(bt.remove(25));
    assert(!bt.find(25, value));
    
    bt.clear();
    
    std::cout << "b_tree tests passed!" << std::endl;
}

void test_graph() {
    std::cout << "Testing graph..." << std::endl;
    
    graph<std::string> g;
    
    for (uint64_t i = 0; i < 10; i++) {
        g.add_vertex(i, "user_" + std::to_string(i));
    }
    
    assert(g.contains_vertex(5));
    
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    
    std::vector<uint64_t> friends;
    g.get_friends(0, friends);
    assert(friends.size() == 2);
    
    g.set_online(0, true);
    assert(g.is_online(0));
    assert(!g.is_online(1));
    
    std::vector<uint64_t> online_friends;
    g.get_online_friends(3, online_friends);
    assert(online_friends.size() == 0);
    
    g.set_online(2, true);
    g.set_online(1, true);
    
    std::vector<uint64_t> mutual;
    g.get_mutual_friends(0, 3, mutual);
    assert(mutual.size() == 2);
    
    std::vector<uint64_t> recommendations;
    g.get_friend_recommendations(0, 5, recommendations);
    
    std::cout << "graph tests passed!" << std::endl;
}

void test_max_heap() {
    std::cout << "Testing max_heap..." << std::endl;
    
    max_heap<int> mh;
    
    assert(mh.empty());
    
    mh.insert(50);
    mh.insert(30);
    mh.insert(70);
    mh.insert(20);
    mh.insert(80);
    
    assert(mh.size() == 5);
    assert(mh.peek_max() == 80);
    
    assert(mh.extract_max() == 80);
    assert(mh.extract_max() == 70);
    assert(mh.size() == 3);
    
    std::vector<int> top;
    mh.get_top_n(2, top);
    assert(top.size() == 2);
    assert(top[0] == 50);
    
    mh.clear();
    assert(mh.empty());
    
    std::cout << "max_heap tests passed!" << std::endl;
}

void test_lru_cache() {
    std::cout << "Testing lru_cache..." << std::endl;
    
    lru_cache<std::string, int> lru(3);
    
    assert(lru.size() == 0);
    
    lru.put("key1", 1);
    lru.put("key2", 2);
    lru.put("key3", 3);
    
    assert(lru.size() == 3);
    assert(lru.contains("key1"));
    
    int value;
    assert(lru.get("key1", value));
    assert(value == 1);
    
    lru.put("key4", 4);
    assert(!lru.contains("key2"));
    assert(lru.contains("key4"));
    
    assert(lru.get("key3", value));
    assert(value == 3);
    
    lru.put("key1", 10);
    assert(lru.get("key1", value));
    assert(value == 10);
    
    lru.clear();
    assert(lru.size() == 0);
    
    std::cout << "lru_cache tests passed!" << std::endl;
}

int main() {
    try {
        test_hash_table();
        test_b_tree();
        test_graph();
        test_max_heap();
        test_lru_cache();
        
        std::cout << "\nAll tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
