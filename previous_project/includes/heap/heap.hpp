#ifndef HEAP_HPP
#define HEAP_HPP

#include <vector>
#include <mutex>
#include <stdexcept>
#include <ctime>

namespace ds
{

struct matchmaking_request
{
    int player_id;
    int elo;
    time_t timestamp;

    // Priority is calculated based on ELO and wait time.
    // A higher value means higher priority.
    double get_priority() const
    {
        double wait_time = difftime(time(nullptr), timestamp);
        // Priority formula: 70% ELO, 30% wait time (wait time becomes more important over time)
        return (0.7 * elo) + (0.3 * wait_time);
    }

    bool operator>(const matchmaking_request& other) const
    {
        return get_priority() > other.get_priority();
    }
};

class max_heap
{
private:
    std::vector<matchmaking_request> heap_;
    mutable std::mutex lock_;

    // Private helper methods for maintaining the heap property
    void heapify_up(int index);
    void heapify_down(int index);

    // Helper methods for navigating the heap
    int parent(int i) { return (i - 1) / 2; }
    int left_child(int i) { return (2 * i) + 1; }
    int right_child(int i) { return (2 * i) + 2; }

public:
    max_heap();
    ~max_heap();

    // Inserts a new request into the heap.
    void insert(const matchmaking_request& request);

    // Returns the highest-priority request without removing it.
    // Throws std::out_of_range if the heap is empty.
    matchmaking_request get_max() const;

    // Removes and returns the highest-priority request.
    // Throws std::out_of_range if the heap is empty.
    matchmaking_request extract_max();

    // Returns the number of requests in the queue.
    size_t size() const;

    // Checks if the queue is empty.
    bool is_empty() const;

    // Deletes all data from the heap.
    void clear();

    // Disable copy constructor and assignment operator
    max_heap(const max_heap&) = delete;
    max_heap& operator=(const max_heap&) = delete;
};

} // namespace ds

#endif // HEAP_HPP
