#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include <unordered_map>
#include <list>
#include <utility>

template<typename K, typename V>
class LRUCache {
private:
    int capacity;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> cache_map;
    std::list<std::pair<K, V>> cache_list;
    
public:
    LRUCache(int cap) : capacity(cap) {}
    
    V Get(const K& key);
    void Put(const K& key, const V& value);
    bool Contains(const K& key) const;
    
private:
    void MoveToFront(const K& key);
};

template<typename K, typename V>
V LRUCache<K, V>::Get(const K& key) {
    if (cache_map.find(key) == cache_map.end()) {
        return V();
    }
    MoveToFront(key);
    return cache_map[key]->second;
}

template<typename K, typename V>
void LRUCache<K, V>::Put(const K& key, const V& value) {
    if (cache_map.find(key) != cache_map.end()) {
        cache_map[key]->second = value;
        MoveToFront(key);
    } else {
        if (cache_list.size() >= capacity) {
            K last_key = cache_list.back().first;
            cache_list.pop_back();
            cache_map.erase(last_key);
        }
        cache_list.push_front({key, value});
        cache_map[key] = cache_list.begin();
    }
}

template<typename K, typename V>
bool LRUCache<K, V>::Contains(const K& key) const {
    return cache_map.find(key) != cache_map.end();
}

template<typename K, typename V>
void LRUCache<K, V>::MoveToFront(const K& key) {
    auto it = cache_map[key];
    if (it != cache_list.begin()) {
        cache_list.erase(it);
        cache_list.push_front(*it);
        cache_map[key] = cache_list.begin();
    }
}

#endif
