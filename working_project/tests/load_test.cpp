#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <cstdint>

#include "../server/src/api/game_state.hpp"

void stress_test_user_operations(game_state& game, int user_count) {
    std::cout << "Starting stress test with " << user_count << " users..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < user_count; i++) {
        std::string username = "user_" + std::to_string(i);
        std::string password = "pass_" + std::to_string(i);
        game.register_user(username, password);
    }
    
    auto after_register = std::chrono::high_resolution_clock::now();
    auto register_time = std::chrono::duration_cast<std::chrono::milliseconds>(after_register - start);
    std::cout << "Registered " << user_count << " users in " << register_time.count() << "ms" << std::endl;
    
    for (int i = 0; i < user_count; i++) {
        std::string username = "user_" + std::to_string(i);
        std::string password = "pass_" + std::to_string(i);
        std::string token;
        game.login_user(username, password, token);
    }
    
    auto after_login = std::chrono::high_resolution_clock::now();
    auto login_time = std::chrono::duration_cast<std::chrono::milliseconds>(after_login - after_register);
    std::cout << "Logged in " << user_count << " users in " << login_time.count() << "ms" << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, user_count - 1);
    
    for (int i = 0; i < user_count / 2; i++) {
        uint64_t user1 = dis(gen) + 1;
        uint64_t user2 = dis(gen) + 1;
        if (user1 != user2) {
            game.send_friend_request(user1, user2);
        }
    }
    
    auto after_friends = std::chrono::high_resolution_clock::now();
    auto friends_time = std::chrono::duration_cast<std::chrono::milliseconds>(after_friends - after_login);
    std::cout << "Added " << user_count / 2 << " friendships in " << friends_time.count() << "ms" << std::endl;
    
    std::cout << "Stress test completed!" << std::endl;
}

int main() {
    game_state game;
    
    stress_test_user_operations(game, 1000);
    
    return 0;
}
