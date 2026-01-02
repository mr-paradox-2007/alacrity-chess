#ifndef MATCHMAKING_QUEUE_HPP
#define MATCHMAKING_QUEUE_HPP

#include <queue>
#include <string>
#include <mutex>

struct QueuedPlayer {
    std::string username;
    int socket_fd;
    long long timestamp;
};

class MatchmakingQueue {
private:
    std::queue<QueuedPlayer> players;
    std::mutex queue_mutex;
    
public:
    void Enqueue(const std::string& username, int socket_fd);
    bool Dequeue(QueuedPlayer& player);
    int GetSize() const { return players.size(); }
    bool IsEmpty() const { return players.empty(); }
    void Clear() { while (!players.empty()) players.pop(); }
};

#endif
