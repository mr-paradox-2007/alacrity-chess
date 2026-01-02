#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <cstdint>

struct user_data {
    uint64_t user_id;
    std::string username;
    std::string password_hash;
    std::string salt;
    int elo_rating;
    int total_matches;
    int wins;
    int losses;
    int draws;
    uint64_t registration_timestamp;
    uint64_t last_login_timestamp;
    std::vector<uint8_t> profile_picture;
    bool is_online;
};

struct user_stats {
    int elo_rating;
    int total_matches;
    int wins;
    int losses;
    int draws;
    double win_rate;
};

#endif
