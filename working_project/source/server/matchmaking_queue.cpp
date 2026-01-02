#include "matchmaking_queue.hpp"
#include <chrono>

void MatchmakingQueue::Enqueue(const std::string& username, int socket_fd) {
    std::unique_lock<std::mutex> lock(queue_mutex);
    QueuedPlayer player;
    player.username = username;
    player.socket_fd = socket_fd;
    player.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    players.push(player);
}

bool MatchmakingQueue::Dequeue(QueuedPlayer& player) {
    std::unique_lock<std::mutex> lock(queue_mutex);
    if (players.empty()) return false;
    player = players.front();
    players.pop();
    return true;
}
