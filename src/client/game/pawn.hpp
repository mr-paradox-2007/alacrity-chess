#ifndef ALACRITY_CHESS_PAWN_HPP
#define ALACRITY_CHESS_PAWN_HPP

#include "piece.hpp"

class pawn : public piece
{
public:
	pawn(position _pos, board* _b, turn _clr);
	void draw(float off_set,float square_size) override;
	bool is_legal_move(position _pos) override;

	int get_type() const override;
};
#endif //ALACRITY_CHESS_PAWN_HPP
