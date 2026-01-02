#include <heap/heap.hpp>
#include <algorithm>

namespace ds
{

max_heap::max_heap()
{
}

max_heap::~max_heap()
{
    clear();
}

void max_heap::insert(const matchmaking_request& request)
{
    std::lock_guard<std::mutex> lock(lock_);

    heap_.push_back(request);
    heapify_up(heap_.size() - 1);
}

void max_heap::heapify_up(int index)
{
    while(index > 0)
    {
        int parent_index = parent(index);

        if(heap_[index] > heap_[parent_index])
        {
            std::swap(heap_[index], heap_[parent_index]);
            index = parent_index;
        }
        else
        {
            break;
        }
    }
}

matchmaking_request max_heap::get_max() const
{
    std::lock_guard<std::mutex> lock(lock_);

    if(heap_.empty())
    {
        throw std::out_of_range("Heap is empty");
    }

    return heap_[0];
}

matchmaking_request max_heap::extract_max()
{
    std::lock_guard<std::mutex> lock(lock_);

    if(heap_.empty())
    {
        throw std::out_of_range("Heap is empty");
    }

    matchmaking_request max_request = heap_[0];

    if(heap_.size() == 1)
    {
        heap_.pop_back();
    }
    else
    {
        heap_[0] = heap_.back();
        heap_.pop_back();
        heapify_down(0);
    }

    return max_request;
}

void max_heap::heapify_down(int index)
{
    int size = heap_.size();

    while(true)
    {
        int largest = index;
        int left = left_child(index);
        int right = right_child(index);

        if(left < size && heap_[left] > heap_[largest])
        {
            largest = left;
        }

        if(right < size && heap_[right] > heap_[largest])
        {
            largest = right;
        }

        if(largest != index)
        {
            std::swap(heap_[index], heap_[largest]);
            index = largest;
        }
        else
        {
            break;
        }
    }
}

size_t max_heap::size() const
{
    std::lock_guard<std::mutex> lock(lock_);
    return heap_.size();
}

bool max_heap::is_empty() const
{
    std::lock_guard<std::mutex> lock(lock_);
    return heap_.empty();
}

void max_heap::clear()
{
    std::lock_guard<std::mutex> lock(lock_);
    heap_.clear();
}

} // namespace ds
