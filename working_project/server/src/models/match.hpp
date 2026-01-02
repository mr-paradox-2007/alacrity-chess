#ifndef MATCH_HPP
#define MATCH_HPP

#include <cstdint>
#include <string>

struct match_data {
    uint64_t match_id;
    uint64_t player1_id;
    uint64_t player2_id;
    uint64_t winner_id;
    int elo_change_p1;
    int elo_change_p2;
    uint64_t timestamp;
    uint64_t duration_seconds;
    int result;
};

#endif
