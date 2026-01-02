#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include <unordered_map>
#include <mutex>
#include <cstdint>

template<typename K, typename V>
class lru_cache {
private:
    struct node {
        K key;
        V value;
        node* prev;
        node* next;
        node(const K& k, const V& v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };
    
    size_t capacity;
    std::unordered_map<K, node*> cache_map;
    node* head;
    node* tail;
    mutable std::mutex mutex_lock;
    
    void move_to_front(node* node_ptr);
    void remove_node(node* node_ptr);
    void add_to_front(node* node_ptr);
    
public:
    lru_cache(size_t cap);
    ~lru_cache();
    
    bool get(const K& key, V& value);
    void put(const K& key, const V& value);
    
    bool contains(const K& key) const;
    size_t size() const;
    void clear();
};

template<typename K, typename V>
lru_cache<K, V>::lru_cache(size_t cap) : capacity(cap) {
    head = new node(K(), V());
    tail = new node(K(), V());
    head->next = tail;
    tail->prev = head;
}

template<typename K, typename V>
lru_cache<K, V>::~lru_cache() {
    std::lock_guard<std::mutex> lock(mutex_lock);
    clear();
    delete head;
    delete tail;
}

template<typename K, typename V>
void lru_cache<K, V>::remove_node(node* node_ptr) {
    node* prev_node = node_ptr->prev;
    node* next_node = node_ptr->next;
    prev_node->next = next_node;
    next_node->prev = prev_node;
}

template<typename K, typename V>
void lru_cache<K, V>::add_to_front(node* node_ptr) {
    node_ptr->next = head->next;
    node_ptr->prev = head;
    head->next->prev = node_ptr;
    head->next = node_ptr;
}

template<typename K, typename V>
void lru_cache<K, V>::move_to_front(node* node_ptr) {
    remove_node(node_ptr);
    add_to_front(node_ptr);
}

template<typename K, typename V>
bool lru_cache<K, V>::get(const K& key, V& value) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it = cache_map.find(key);
    if (it == cache_map.end()) return false;
    node* node_ptr = it->second;
    move_to_front(node_ptr);
    value = node_ptr->value;
    return true;
}

template<typename K, typename V>
void lru_cache<K, V>::put(const K& key, const V& value) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    auto it = cache_map.find(key);
    if (it != cache_map.end()) {
        node* node_ptr = it->second;
        node_ptr->value = value;
        move_to_front(node_ptr);
        return;
    }
    node* new_node = new node(key, value);
    add_to_front(new_node);
    cache_map[key] = new_node;
    if (cache_map.size() > capacity) {
        node* to_remove = tail->prev;
        remove_node(to_remove);
        cache_map.erase(to_remove->key);
        delete to_remove;
    }
}

template<typename K, typename V>
bool lru_cache<K, V>::contains(const K& key) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    return cache_map.find(key) != cache_map.end();
}

template<typename K, typename V>
size_t lru_cache<K, V>::size() const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    return cache_map.size();
}

template<typename K, typename V>
void lru_cache<K, V>::clear() {
    node* current = head->next;
    while (current != tail) {
        node* temp = current;
        current = current->next;
        delete temp;
    }
    head->next = tail;
    tail->prev = head;
    cache_map.clear();
}

#endif
