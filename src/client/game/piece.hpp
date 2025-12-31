#ifndef ALACRITY_CHESS_PIECE_HPP
#define ALACRITY_CHESS_PIECE_HPP

#include "utility.hpp"

class board;

class piece
{
protected:
    position pos;
    board* b;
    turn clr;


    int abs(int x);
    bool is_horizontal_move(position _pos);
    bool is_vertical_move(position _pos);
    bool is_diagonal_move(position _pos);
    bool is_path_clear_horizontal(position _pos);
    bool is_path_clear_vertical(position _pos);
    bool is_path_clear_diagonal(position _pos);

public:
    piece(position _pos, board* _b, turn _clr);
    virtual ~piece();
    virtual void draw(float off_set,float square_size) = 0;
    virtual bool is_legal_move(position _pos) = 0;
    virtual int get_type() const = 0;
    void move(position _pos);
    position get_pos();
    turn get_clr();
    board* get_b();
};


#endif //ALACRITY_CHESS_PIECE_HPP
