#ifndef MAX_HEAP_HPP
#define MAX_HEAP_HPP

#include <vector>
#include <mutex>
#include <algorithm>
#include <cstdint>
#include <stdexcept>

template<typename T>
class max_heap {
private:
    std::vector<T> heap;
    mutable std::mutex mutex_lock;
    
    void heapify_up(size_t index);
    void heapify_down(size_t index);
    size_t parent_index(size_t index) const;
    size_t left_child_index(size_t index) const;
    size_t right_child_index(size_t index) const;
    
public:
    max_heap() = default;
    
    void insert(const T& value);
    T extract_max();
    T peek_max() const;
    
    void get_top_n(int n, std::vector<T>& result) const;
    
    size_t size() const;
    bool empty() const;
    
    void clear();
    
    std::vector<T> to_vector() const;
};

template<typename T>
size_t max_heap<T>::parent_index(size_t index) const {
    return (index - 1) / 2;
}

template<typename T>
size_t max_heap<T>::left_child_index(size_t index) const {
    return 2 * index + 1;
}

template<typename T>
size_t max_heap<T>::right_child_index(size_t index) const {
    return 2 * index + 2;
}

template<typename T>
void max_heap<T>::heapify_up(size_t index) {
    while (index > 0 && heap[parent_index(index)] < heap[index]) {
        std::swap(heap[index], heap[parent_index(index)]);
        index = parent_index(index);
    }
}

template<typename T>
void max_heap<T>::heapify_down(size_t index) {
    while (true) {
        size_t largest = index;
        size_t left = left_child_index(index);
        size_t right = right_child_index(index);
        if (left < heap.size() && heap[left] > heap[largest]) largest = left;
        if (right < heap.size() && heap[right] > heap[largest]) largest = right;
        if (largest != index) {
            std::swap(heap[index], heap[largest]);
            index = largest;
        } else break;
    }
}

template<typename T>
void max_heap<T>::insert(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    heap.push_back(value);
    heapify_up(heap.size() - 1);
}

template<typename T>
T max_heap<T>::extract_max() {
    std::lock_guard<std::mutex> lock(mutex_lock);
    if (heap.empty()) throw std::runtime_error("Heap is empty");
    T max_val = heap[0];
    heap[0] = heap[heap.size() - 1];
    heap.pop_back();
    if (!heap.empty()) heapify_down(0);
    return max_val;
}

template<typename T>
T max_heap<T>::peek_max() const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    if (heap.empty()) throw std::runtime_error("Heap is empty");
    return heap[0];
}

template<typename T>
void max_heap<T>::get_top_n(int n, std::vector<T>& result) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    std::vector<T> temp_heap = heap;
    int count = 0;
    while (!temp_heap.empty() && count < n) {
        result.push_back(temp_heap[0]);
        temp_heap[0] = temp_heap[temp_heap.size() - 1];
        temp_heap.pop_back();
        if (!temp_heap.empty()) {
            size_t index = 0;
            while (true) {
                size_t largest = index;
                size_t left = 2 * index + 1;
                size_t right = 2 * index + 2;
                if (left < temp_heap.size() && temp_heap[left] > temp_heap[largest]) largest = left;
                if (right < temp_heap.size() && temp_heap[right] > temp_heap[largest]) largest = right;
                if (largest != index) {
                    std::swap(temp_heap[index], temp_heap[largest]);
                    index = largest;
                } else break;
            }
        }
        count++;
    }
}

template<typename T>
size_t max_heap<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    return heap.size();
}

template<typename T>
bool max_heap<T>::empty() const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    return heap.empty();
}

template<typename T>
void max_heap<T>::clear() {
    std::lock_guard<std::mutex> lock(mutex_lock);
    heap.clear();
}

template<typename T>
std::vector<T> max_heap<T>::to_vector() const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    return heap;
}

#endif
