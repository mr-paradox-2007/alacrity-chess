#ifndef SESSION_HPP
#define SESSION_HPP

#include <string>
#include <cstdint>

struct session_data {
    std::string token;
    uint64_t user_id;
    uint64_t created_at;
    uint64_t expires_at;
    bool is_valid;
};

#endif
