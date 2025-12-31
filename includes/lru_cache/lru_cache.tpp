#ifndef LRU_CACHE_TPP
#define LRU_CACHE_TPP

namespace ds
{

template <typename TKey, typename TValue>
lru_cache<TKey, TValue>::lru_cache(size_t capacity) : capacity_(capacity)
{
}

template <typename TKey, typename TValue>
lru_cache<TKey, TValue>::~lru_cache()
{
    clear();
}

template <typename TKey, typename TValue>
void lru_cache<TKey, TValue>::put(const TKey& key, const TValue& value)
{
    std::lock_guard<std::mutex> lock(lock_);

    // Check if key already exists
    auto it = cache_map_.find(key);
    if(it != cache_map_.end())
    {
        // Key exists, erase the old item from the list
        lru_list_.erase(it->second);
        cache_map_.erase(it);
    }
    else if(lru_list_.size() >= capacity_)
    {
        // Cache is full, evict the least recently used (back of list)
        const TKey& lru_key = lru_list_.back().first;
        cache_map_.erase(lru_key);
        lru_list_.pop_back();
    }

    // Push the new key-value pair to the front of lru_list_
    lru_list_.push_front(std::make_pair(key, value));

    // Store the iterator to the new list item in cache_map_
    cache_map_[key] = lru_list_.begin();
}

template <typename TKey, typename TValue>
std::optional<TValue> lru_cache<TKey, TValue>::get(const TKey& key)
{
    std::lock_guard<std::mutex> lock(lock_);

    auto it = cache_map_.find(key);
    if(it == cache_map_.end())
    {
        return std::nullopt;
    }

    // Get the value from the iterator
    TValue value = it->second->second;

    // Move the found node to the front (most recently used)
    lru_list_.splice(lru_list_.begin(), lru_list_, it->second);

    return value;
}

template <typename TKey, typename TValue>
size_t lru_cache<TKey, TValue>::size() const
{
    std::lock_guard<std::mutex> lock(lock_);
    return lru_list_.size();
}

template <typename TKey, typename TValue>
bool lru_cache<TKey, TValue>::exists(const TKey& key) const
{
    std::lock_guard<std::mutex> lock(lock_);
    return cache_map_.find(key) != cache_map_.end();
}

template <typename TKey, typename TValue>
void lru_cache<TKey, TValue>::clear()
{
    std::lock_guard<std::mutex> lock(lock_);
    lru_list_.clear();
    cache_map_.clear();
}

} // namespace ds

#endif // LRU_CACHE_TPP
