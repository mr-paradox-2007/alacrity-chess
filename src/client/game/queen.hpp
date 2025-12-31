#ifndef ALACRITY_CHESS_QUEEN_HPP
#define ALACRITY_CHESS_QUEEN_HPP

#include "piece.hpp"

class queen : public piece
{
public:
    queen(position _pos, board* _b, turn _clr);
    void draw(float off_set, float square_size) override;
    bool is_legal_move(position _pos) override;

    int get_type() const override;
};

#endif //ALACRITY_CHESS_QUEEN_HPP
