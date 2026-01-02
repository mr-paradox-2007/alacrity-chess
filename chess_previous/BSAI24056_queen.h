#ifndef BSAI24056_QUEEN_H
#define BSAI24056_QUEEN_H

#include "BSAI24056_piece.h"

class queen : public piece
{
public:
    queen(position _pos, board* _b, turn _clr);
    void draw(float off_set, float square_size) override;
    bool is_legal_move(position _pos) override;

    int get_type() const override;
};

#endif //BSAI24056_QUEEN_H