#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include <list>
#include <unordered_map>
#include <mutex>
#include <optional>

namespace ds
{

template <typename TKey, typename TValue>
class lru_cache
{
private:
    // The list stores pairs of (key, value) to track usage order.
    // Most recently used is at the front, least recently used is at the back.
    std::list<std::pair<TKey, TValue>> lru_list_;

    // The map provides O(1) lookup for list iterators based on the key.
    std::unordered_map<TKey, typename std::list<std::pair<TKey, TValue>>::iterator> cache_map_;

    size_t capacity_;
    mutable std::mutex lock_;

public:
    // Constructor to initialize the cache with a given capacity.
    explicit lru_cache(size_t capacity);
    ~lru_cache();

    // Adds a key-value pair to the cache.
    // If the key already exists, its value is updated, and it's moved to the front (most recent).
    // If the cache is full, the least recently used item is evicted.
    void put(const TKey& key, const TValue& value);

    // Retrieves a value by its key.
    // If the key is found, the item is moved to the front, and its value is returned.
    // Uses std::optional to handle cache misses gracefully without throwing exceptions.
    std::optional<TValue> get(const TKey& key);

    // Returns the current number of items in the cache.
    size_t size() const;

    // Checks if the cache contains a specific key.
    bool exists(const TKey& key) const;

    // Removes all items from the cache.
    void clear();

    // Disable copy constructor and assignment operator
    lru_cache(const lru_cache&) = delete;
    lru_cache& operator=(const lru_cache&) = delete;
};

} // namespace ds

#include "lru_cache.tpp" // Include template implementation

#endif // LRU_CACHE_HPP
