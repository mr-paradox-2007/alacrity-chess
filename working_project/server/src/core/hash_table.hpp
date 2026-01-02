#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <string>
#include <mutex>
#include <functional>
#include <cstdint>
#include <vector>
#include <stdexcept>

template<typename K, typename V>
class hash_table {
private:
    struct node {
        K key;
        V value;
        node* next;
        node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };
    
    node** buckets;
    size_t capacity;
    size_t count;
    mutable std::mutex mutex_lock;
    
    size_t hash_function(const K& key) const;
    void resize();
    
public:
    hash_table(size_t initial_capacity = 1024);
    ~hash_table();
    
    bool insert(const K& key, const V& value);
    bool find(const K& key, V& value) const;
    bool remove(const K& key);
    bool update(const K& key, const V& value);
    size_t size() const;
    bool empty() const;
    bool contains(const K& key) const;
    
    void clear();
    
    void serialize(const std::string& filename) const;
    void deserialize(const std::string& filename);
    
    template<typename Func>
    void iterate(Func callback) const;
};

template<typename K, typename V>
size_t hash_table<K, V>::hash_function(const K& key) const {
    return std::hash<K>()(key) % capacity;
}

template<typename K, typename V>
hash_table<K, V>::hash_table(size_t initial_capacity) 
    : capacity(initial_capacity), count(0) {
    buckets = new node*[capacity];
    for (size_t i = 0; i < capacity; i++) {
        buckets[i] = nullptr;
    }
}

template<typename K, typename V>
hash_table<K, V>::~hash_table() {
    std::lock_guard<std::mutex> lock(mutex_lock);
    for (size_t i = 0; i < capacity; i++) {
        node* current = buckets[i];
        while (current) {
            node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] buckets;
}

template<typename K, typename V>
void hash_table<K, V>::resize() {
    size_t old_capacity = capacity;
    node** old_buckets = buckets;
    
    capacity *= 2;
    buckets = new node*[capacity];
    for (size_t i = 0; i < capacity; i++) {
        buckets[i] = nullptr;
    }
    
    size_t old_count = count;
    count = 0;
    for (size_t i = 0; i < old_capacity; i++) {
        node* current = old_buckets[i];
        while (current) {
            size_t new_index = std::hash<K>()(current->key) % capacity;
            node* next_node = current->next;
            current->next = buckets[new_index];
            buckets[new_index] = current;
            count++;
            current = next_node;
        }
    }
    
    delete[] old_buckets;
}

template<typename K, typename V>
bool hash_table<K, V>::insert(const K& key, const V& value) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    
    if (static_cast<double>(count) / capacity > 0.75) {
        resize();
    }
    
    size_t index = hash_function(key);
    node* current = buckets[index];
    
    while (current) {
        if (current->key == key) {
            return false;
        }
        current = current->next;
    }
    
    node* new_node = new node(key, value);
    new_node->next = buckets[index];
    buckets[index] = new_node;
    count++;
    
    return true;
}

template<typename K, typename V>
bool hash_table<K, V>::find(const K& key, V& value) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    
    size_t index = hash_function(key);
    node* current = buckets[index];
    
    while (current) {
        if (current->key == key) {
            value = current->value;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

template<typename K, typename V>
bool hash_table<K, V>::contains(const K& key) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    
    size_t index = hash_function(key);
    node* current = buckets[index];
    
    while (current) {
        if (current->key == key) {
            return true;
        }
        current = current->next;
    }
    
    return false;
}

template<typename K, typename V>
bool hash_table<K, V>::remove(const K& key) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    
    size_t index = hash_function(key);
    node* current = buckets[index];
    node* prev = nullptr;
    
    while (current) {
        if (current->key == key) {
            if (prev) {
                prev->next = current->next;
            } else {
                buckets[index] = current->next;
            }
            delete current;
            count--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false;
}

template<typename K, typename V>
bool hash_table<K, V>::update(const K& key, const V& value) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    
    size_t index = hash_function(key);
    node* current = buckets[index];
    
    while (current) {
        if (current->key == key) {
            current->value = value;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

template<typename K, typename V>
size_t hash_table<K, V>::size() const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    return count;
}

template<typename K, typename V>
bool hash_table<K, V>::empty() const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    return count == 0;
}

template<typename K, typename V>
void hash_table<K, V>::clear() {
    std::lock_guard<std::mutex> lock(mutex_lock);
    for (size_t i = 0; i < capacity; i++) {
        node* current = buckets[i];
        while (current) {
            node* temp = current;
            current = current->next;
            delete temp;
        }
        buckets[i] = nullptr;
    }
    count = 0;
}

template<typename K, typename V>
template<typename Func>
void hash_table<K, V>::iterate(Func callback) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    for (size_t i = 0; i < capacity; i++) {
        node* current = buckets[i];
        while (current) {
            callback(current->key, current->value);
            current = current->next;
        }
    }
}

template<typename K, typename V>
void hash_table<K, V>::serialize(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
}

template<typename K, typename V>
void hash_table<K, V>::deserialize(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_lock);
}

#endif
