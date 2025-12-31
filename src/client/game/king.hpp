#ifndef ALACRITY_CHESS_KING_HPP
#define ALACRITY_CHESS_KING_HPP

#include "piece.hpp"

class king : public piece
{
public:
    king(position _pos, board* _b, turn _clr);
    void draw(float off_set, float square_size) override;
    bool is_legal_move(position _pos) override;

    bool is_castling_move(position _pos);
    int get_type() const override;
};

#endif //ALACRITY_CHESS_KING_HPP
