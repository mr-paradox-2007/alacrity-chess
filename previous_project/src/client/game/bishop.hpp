#ifndef ALACRITY_CHESS_BISHOP_HPP
#define ALACRITY_CHESS_BISHOP_HPP

#include "piece.hpp"

class bishop : public piece
{
public:
    bishop(position _pos, board* _b, turn _clr);
    void draw(float off_set, float square_size) override;
    bool is_legal_move(position _pos) override;
    int get_type() const override;
};

#endif //ALACRITY_CHESS_BISHOP_HPP
