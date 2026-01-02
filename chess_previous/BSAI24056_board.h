#ifndef BSAI24056_BOARD_H
#define BSAI24056_BOARD_H

#include "BSAI24056_utility.h"
#include "BSAI24056_piece.h"
#include "BSAI24056_pawn.h"
#include "BSAI24056_bishop.h"
#include "BSAI24056_rook.h"
#include "BSAI24056_knight.h"
#include "BSAI24056_affan_bhai.h"   
#include "BSAI24056_queen.h"

class board
{
protected:
    turn current_turn;
    piece* pieces[8][8];

    bool piece_moved[8][8]; 
    int active_palette_index;
    void init_pieces(bool reseted);
    void set_palette(int index);
    position find_king();
    position en_passant_target;

public:
    board();
    ~board();
    void print_board();
    piece* get_piece(position _pos);
    void cycle_palette();
    void draw_pieces();
    bool is_valid_source(position _pos);
    bool is_valid_destination(position _pos);
    turn get_current_turn() const;
    bool is_legal(position s_pos, position d_pos);
    void move_piece(position s_pos, position d_pos);
    bool is_check();
    bool is_checkmate();
    void change_turn();
    bool would_be_in_check_after_move(position s_pos, position d_pos);
    bool is_stalemate();
    void promote_pawn(position _pos, int promotion_type);
    void reset(bool reseted);

    
    bool can_castle_kingside(turn color);
    bool can_castle_queenside(turn color);
    void perform_castle(bool kingside);
    bool piece_has_moved(position pos);

    bool has_king_moved(int color);
    bool has_rook_moved(int color, bool kingside);
    position get_en_passant_target();

    void place_piece(position to, int type, int color);

    void set_king_moved(int color, bool moved);
    void set_rook_moved(int color, bool kingside, bool moved);
    void set_en_passant_target(position target);

};

#endif //BSAI24056_BOARD_H