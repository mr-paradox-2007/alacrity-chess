#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <string>
#include <vector>

template<typename K, typename V>
class HashTable {
private:
    static const int CAPACITY = 1000;
    
    struct Entry {
        K key;
        V value;
        bool occupied;
        Entry() : occupied(false) {}
    };
    
    std::vector<Entry> table;
    
    int Hash(const K& key) const {
        return std::hash<K>{}(key) % CAPACITY;
    }
    
public:
    HashTable() : table(CAPACITY) {}
    
    void Insert(const K& key, const V& value) {
        int index = Hash(key);
        int attempts = 0;
        while (attempts < CAPACITY && table[index].occupied) {
            if (table[index].key == key) {
                table[index].value = value;
                return;
            }
            index = (index + 1) % CAPACITY;
            attempts++;
        }
        if (attempts < CAPACITY) {
            table[index].key = key;
            table[index].value = value;
            table[index].occupied = true;
        }
    }
    
    bool Find(const K& key, V& value) const {
        int index = Hash(key);
        int attempts = 0;
        while (attempts < CAPACITY && table[index].occupied) {
            if (table[index].key == key) {
                value = table[index].value;
                return true;
            }
            index = (index + 1) % CAPACITY;
            attempts++;
        }
        return false;
    }
    
    bool Contains(const K& key) const {
        int index = Hash(key);
        int attempts = 0;
        while (attempts < CAPACITY && table[index].occupied) {
            if (table[index].key == key) return true;
            index = (index + 1) % CAPACITY;
            attempts++;
        }
        return false;
    }
    
    void Remove(const K& key) {
        int index = Hash(key);
        int attempts = 0;
        while (attempts < CAPACITY && table[index].occupied) {
            if (table[index].key == key) {
                table[index].occupied = false;
                return;
            }
            index = (index + 1) % CAPACITY;
            attempts++;
        }
    }
};

#endif
