#ifndef FRIEND_REQUEST_HPP
#define FRIEND_REQUEST_HPP

#include <cstdint>

struct friend_request_data {
    uint64_t request_id;
    uint64_t sender_id;
    uint64_t receiver_id;
    int status;
    uint64_t created_at;
};

#endif
