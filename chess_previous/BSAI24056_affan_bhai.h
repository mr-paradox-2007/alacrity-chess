#ifndef BSAI24056_KING_H
#define BSAI24056_KING_H

#include "BSAI24056_piece.h"

class king : public piece
{
public:
    king(position _pos, board* _b, turn _clr);
    void draw(float off_set, float square_size) override;
    bool is_legal_move(position _pos) override;

    bool is_castling_move(position _pos);
    int get_type() const override;
};

#endif //BSAI24056_KING_H