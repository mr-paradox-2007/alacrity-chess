#include "BSAI24056_move_history.h"

void move_history::add_move(const chess_move& move)
{
    if (current_move_index < (int)moves.size() - 1)
    {
        moves.erase(moves.begin() + current_move_index + 1, moves.end());
    }

    moves.push_back(move);
    current_move_index = moves.size() - 1;
}

bool move_history::undo_move() 
{
    if (!can_undo()) return false;
    current_move_index--;
    return true;
}

bool move_history::redo_move() 
{
    if (!can_redo()) return false;
    current_move_index++;
    return true;
}

void move_history::reset_to_start() 
{
    current_move_index = -1;
}

bool move_history::step_forward() 
{
    return redo_move();
}

bool move_history::step_backward() 
{
    return undo_move();
}

bool move_history::jump_to_move(int move_number) 
{
    if (move_number < -1 || move_number >= (int)moves.size())
    {
        return false;
    }
    current_move_index = move_number;
    return true;
}

const chess_move* move_history::get_move(int index) const
{
    if (index < 0 || index >= (int)moves.size())
    {
        return nullptr;
    }
    return &moves[index];
}

std::string move_history::move_to_string(int index) const
{
    const chess_move* move = get_move(index);
    if (!move) return "";

    char from_str[3], to_str[3];
    sprintf_s(from_str, "%c%d", 'a' + move->from.y, 8 - move->from.x);
    sprintf_s(to_str, "%c%d", 'a' + move->to.y, 8 - move->to.x);

    std::string result = std::string(from_str) + "-" + std::string(to_str);

    if (move->was_castling) {
        result += " (O-O)";
    }
    else if (move->was_promotion) {
        result += " (Promotion)";
    }
    else if (move->captured_piece_type != -1) {
        result += " (Capture)";
    }

    return result;
}

bool move_history::save_to_file(const string& filename) const 
{
    ofstream file(filename);
    if (!file.is_open()) return false;

    file << moves.size() << "\n";

    for (const auto& move : moves) 
    {
        file << move.from.x << " " << move.from.y << " "
            << move.to.x << " " << move.to.y << " "
            << move.piece_type << " " << move.piece_color << " "
            << move.captured_piece_type << " " << move.captured_piece_color << " "
            << move.was_castling << " " << move.was_en_passant << " "
            << move.was_promotion << " " << move.promotion_piece_type << " "
            << move.king_moved_before << " " << move.rook_kingside_moved_before << " "
            << move.rook_queenside_moved_before << " "
            << move.en_passant_target.x << " " << move.en_passant_target.y << "\n";
    }

    return true;
}

bool move_history::load_from_file(const string& filename) 
{
    ifstream file(filename);
    if (!file.is_open()) return false;

    moves.clear();
    current_move_index = -1;

    int num_moves;
    file >> num_moves;

    for (int i = 0; i < num_moves; i++) {
        chess_move move;
        file >> move.from.x >> move.from.y
            >> move.to.x >> move.to.y
            >> move.piece_type >> move.piece_color
            >> move.captured_piece_type >> move.captured_piece_color
            >> move.was_castling >> move.was_en_passant
            >> move.was_promotion >> move.promotion_piece_type
            >> move.king_moved_before >> move.rook_kingside_moved_before
            >> move.rook_queenside_moved_before
            >> move.en_passant_target.x >> move.en_passant_target.y;

        moves.push_back(move);
    }

    current_move_index = moves.size() - 1; 
    return true;
}