#ifndef ALACRITY_CHESS_MOVE_HISTORY_HPP
#define ALACRITY_CHESS_MOVE_HISTORY_HPP

#include "utility.hpp"
#include <string>
#include <vector>
#include <fstream>

struct chess_move 
{
    position from;
    position to;
    int piece_type;           
    int piece_color;          
    int captured_piece_type;  
    int captured_piece_color; 
    bool was_castling;        
    bool was_en_passant;
    bool was_promotion;
    int promotion_piece_type; 

    bool king_moved_before;   
    bool rook_kingside_moved_before;
    bool rook_queenside_moved_before;
    position en_passant_target; 

    chess_move() : from(-1, -1), to(-1, -1), piece_type(-1), piece_color(-1),
        captured_piece_type(-1), captured_piece_color(-1),
        was_castling(false), was_en_passant(false), was_promotion(false),
        promotion_piece_type(-1), king_moved_before(false),
        rook_kingside_moved_before(false), rook_queenside_moved_before(false),
        en_passant_target(-1, -1) {}
};

class move_history 
{
private:
    std::vector<chess_move> moves;
    int current_move_index; 

public:
    move_history() : current_move_index(-1) {}

    void add_move(const chess_move& move);
    bool undo_move();
    bool redo_move();
    void clear_history();

    void reset_to_start();
    bool step_forward();
    bool step_backward();
    bool jump_to_move(int move_number);

    int get_current_move_index() const 
    {
        return current_move_index; 
    }
    int get_total_moves() const 
    {
        return moves.size(); 
    }
    bool can_undo() const 
    { 
        return current_move_index >= 0; 
    }
    bool can_redo() const 
    { 
        return current_move_index < (int)moves.size() - 1; 
    }

    bool save_to_file(const std::string& filename) const;
    bool load_from_file(const std::string& filename);

    const chess_move* get_move(int index) const;
    std::string move_to_string(int index) const; 
};

#endif //ALACRITY_CHESS_MOVE_HISTORY_HPP
