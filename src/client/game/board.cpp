#include "board.hpp"
#include <vector>
#include <algorithm> // For std::max

struct BoardPalette {
    Color light;
    Color dark;
    Color border;
    Color highlight;
    Color border_top;
    const char* name;
};
const BoardPalette board_palettes[] = {
    // Classic Elegant
    {
        Color{220, 210, 195, 225},  // light squares
        Color{120, 85, 60, 225},    // dark squares
        Color{95, 65, 45, 170},     // border
        Color{235, 225, 210, 130},  // highlight
        Color{135, 95, 65, 190},    // border top
        "Classic Elegant"
    },

    // Marble Effect
    {
        Color{230, 225, 220, 225},  // light squares
        Color{130, 95, 75, 225},    // dark squares
        Color{100, 70, 50, 170},    // border
        Color{240, 235, 225, 130},  // highlight
        Color{140, 100, 75, 190},   // border top
        "Marble Effect"
    },

    // Royal
    {
        Color{235, 220, 200, 225},  // light squares
        Color{140, 80, 60, 225},    // dark squares
        Color{110, 60, 40, 180},    // border
        Color{245, 230, 210, 140},  // highlight
        Color{150, 85, 55, 200},    // border top
        "Royal"
    },

    // Modern Minimalist
    {
        Color{230, 230, 230, 225},  // light squares
        Color{100, 100, 110, 225},  // dark squares
        Color{80, 80, 90, 170},     // border
        Color{240, 240, 240, 130},  // highlight
        Color{90, 90, 100, 190},    // border top
        "Modern Minimalist"
    },

    // Rich Wood
    {
        Color{225, 205, 175, 225},  // light squares
        Color{145, 95, 55, 225},    // dark squares
        Color{120, 75, 40, 180},    // border
        Color{235, 215, 185, 140},  // highlight
        Color{160, 105, 65, 200},   // border top
        "Rich Wood"
    },

    // Metallic
    {
        Color{180, 185, 190, 225},  // light squares
        Color{90, 95, 100, 225},    // dark squares
        Color{70, 75, 80, 180},     // border
        Color{200, 205, 210, 140},  // highlight
        Color{100, 105, 110, 200},  // border top
        "Metallic"
    },

    // Jade and Rosewood
    {
        Color{215, 235, 220, 225},  // light squares
        Color{80, 45, 40, 225},     // dark squares
        Color{60, 35, 30, 180},     // border
        Color{225, 245, 230, 140},  // highlight
        Color{90, 55, 45, 200},     // border top
        "Jade and Rosewood"
    },

    // Twilight
    {
        Color{220, 215, 195, 225},  // light squares
        Color{80, 80, 120, 225},    // dark squares
        Color{60, 60, 100, 180},    // border
        Color{230, 225, 205, 140},  // highlight
        Color{90, 90, 130, 200},    // border top
        "Twilight"
    },

    // Walnut and Maple
    {
        Color{230, 215, 185, 225},  // light squares
        Color{120, 70, 40, 225},    // dark squares
        Color{100, 60, 35, 180},    // border
        Color{240, 225, 195, 130},  // highlight
        Color{140, 80, 45, 200},    // border top
        "Walnut and Maple"
    },

    // Emerald and Ebony
    {
        Color{200, 230, 200, 225},  // light squares
        Color{40, 40, 40, 225},     // dark squares
        Color{30, 30, 30, 180},     // border
        Color{210, 240, 210, 130},  // highlight
        Color{50, 50, 50, 200},     // border top
        "Emerald and Ebony"
    },

    // Cherry and Ash
    {
        Color{225, 220, 210, 225},  // light squares
        Color{160, 70, 70, 225},    // dark squares
        Color{140, 60, 60, 180},    // border
        Color{235, 230, 220, 130},  // highlight
        Color{180, 80, 80, 200},    // border top
        "Cherry and Ash"
    },

    // Blue and Gold
    {
        Color{220, 210, 180, 225},  // light squares
        Color{60, 90, 120, 225},    // dark squares
        Color{50, 75, 100, 180},    // border
        Color{230, 220, 190, 130},  // highlight
        Color{70, 100, 130, 200},   // border top
        "Blue and Gold"
    }
};

board::board()
{
    active_palette_index = 0;
    current_turn = white;
    en_passant_target = position(-1, -1); 
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            pieces[x][y] = nullptr;
            piece_moved[x][y] = false;
        }
    }
}

board::~board()
{
    // Clean up all allocated pieces
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (pieces[x][y] != nullptr)
            {
                delete pieces[x][y];
                pieces[x][y] = nullptr;
            }
        }
    }
}

void board::reset(bool reseted)
{
    // Clear existing pieces before re-initialization
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (pieces[x][y] != nullptr)
            {
                delete pieces[x][y];
                pieces[x][y] = nullptr;
            }
        }
    }
    init_pieces(reseted);
}

void board::print_board()
{
    const BoardPalette& palette = board_palettes[active_palette_index];

    float board_height = 0.45f;

    for (int x = -1; x <= N; x++)
    {
        for (int z = -1; z <= N; z++) 
        {
            if (x == -1 || x == N || z == -1 || z == N)
            {
                Vector3 border_pos = { off + x * sq, 0.0f, off + z * sq };
                DrawCube(border_pos, sq, board_height, sq, palette.border);

                Vector3 border_top_pos = { border_pos.x, board_height / 2, border_pos.z };
                DrawCube(border_top_pos, sq, 0.01f, sq, palette.border_top);
            }
        }
    }

    for (int x = 0; x < N; x++) 
    {
        for (int z = 0; z < N; z++) 
        {
            Vector3 pos = { off + x * sq, board_height / 2.0f, off + z * sq };
            bool is_light = ((x + z) % 2) == 0;
            Color square_color = is_light ? palette.light : palette.dark;

            DrawCube(pos, sq, board_height, sq, square_color);

            if (is_light) 
            {
                Vector3 highlight = { pos.x, pos.y + board_height / 2.0f + 0.001f, pos.z };
                DrawCube(highlight, sq, 0.01f, sq, palette.highlight);
            }

            Vector3 border = { pos.x, pos.y, pos.z };
            DrawCubeWires(border, sq, board_height, sq, Color{ 0, 0, 0, 100 });
        }
    }
}

void board::set_palette(int index)
{
    int num_palettes = sizeof(board_palettes) / sizeof(board_palettes[0]);
    active_palette_index = index % num_palettes;
    if (active_palette_index < 0) active_palette_index = num_palettes - 1;
}

void board::cycle_palette()
{
    int num_palettes = sizeof(board_palettes) / sizeof(board_palettes[0]);
    active_palette_index = (active_palette_index + 1) % num_palettes;
}

piece* board::get_piece(position _pos)
{
    return pieces[_pos.x][_pos.y];
}

void board::init_pieces(bool reseted)
{
    // The previous loop to delete pieces is redundant here as reset() already does it.
    // However, if reseted is false, then we should re-create pieces without clearing first.
    // For now, assuming reseted means a full board clear and re-setup.

    // White pawns
    for (int y = 0; y < 8; y++)
    {
        pieces[6][y] = new pawn({ 6, y }, this, white);
    }

    // Black pawns
    for (int y = 0; y < 8; y++)
    {
        pieces[1][y] = new pawn({ 1, y }, this, black);
    }

    // Black back-rank
    pieces[0][0] = new rook({ 0, 0 }, this, black);
    pieces[0][1] = new knight({ 0, 1 }, this, black);
    pieces[0][2] = new bishop({ 0, 2 }, this, black);
    pieces[0][3] = new queen({ 0, 3 }, this, black);
    pieces[0][4] = new king({ 0, 4 }, this, black);
    pieces[0][5] = new bishop({ 0, 5 }, this, black);
    pieces[0][6] = new knight({ 0, 6 }, this, black);
    pieces[0][7] = new rook({ 0, 7 }, this, black);

    // White back-rank
    pieces[7][0] = new rook({ 7, 0 }, this, white);
    pieces[7][1] = new knight({ 7, 1 }, this, white);
    pieces[7][2] = new bishop({ 7, 2 }, this, white);
    pieces[7][3] = new queen({ 7, 3 }, this, white);
    pieces[7][4] = new king({ 7, 4 }, this, white);
    pieces[7][5] = new bishop({ 7, 5 }, this, white);
    pieces[7][6] = new knight({ 7, 6 }, this, white);
    pieces[7][7] = new rook({ 7, 7 }, this, white);
    
    // Reset piece_moved flags
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            piece_moved[x][y] = false;
        }
    }
}

void board::draw_pieces()
{
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (pieces[x][y] != nullptr)
            {
                pieces[x][y]->draw(off, sq);
            }
        }
    }
}

bool board::is_valid_source(position _pos)
{
    if (_pos.x < 0 || _pos.x > 7 || _pos.y < 0 || _pos.y > 7)
        return false;
    if (pieces[_pos.x][_pos.y] == nullptr)
        return false;

    return pieces[_pos.x][_pos.y]->get_clr() == current_turn;
}

bool board::is_valid_destination(position _pos)
{
    if (_pos.x < 0 || _pos.x > 7 || _pos.y < 0 || _pos.y > 7)
        return false;

    return (pieces[_pos.x][_pos.y] == nullptr || pieces[_pos.x][_pos.y]->get_clr() != current_turn);
}

position board::find_king()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (pieces[i][j] != nullptr && pieces[i][j]->get_clr() == current_turn &&
                dynamic_cast<king*>(pieces[i][j]) != nullptr)
            {
                return position(i, j);
            }
        }
    }
    return position(-1, -1); // Should not happen in a valid game state
}

bool board::is_check()
{
    position king_pos = find_king();

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (pieces[i][j] != nullptr && pieces[i][j]->get_clr() != current_turn)
            {
                // Temporarily change turn to check opponent's moves
                turn original_turn = current_turn;
                current_turn = pieces[i][j]->get_clr(); // Set current_turn to attacker's color
                bool can_attack = pieces[i][j]->is_legal_move(king_pos);
                current_turn = original_turn; // Revert turn
                if (can_attack)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

turn board::get_current_turn() const
{
    return current_turn;
}

bool board::is_checkmate()
{
    if (!is_check())
        return false;

    position king_pos = find_king();

    // 1. Can the king move out of check?
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0)
                continue;

            position new_pos(king_pos.x + dx, king_pos.y + dy);

            if (new_pos.x >= 0 && new_pos.x < 8 && new_pos.y >= 0 && new_pos.y < 8)
            {
                // Check if the move is valid and doesn't result in check
                if (is_valid_destination(new_pos))
                {
                    piece* king_piece = pieces[king_pos.x][king_pos.y];
                    if (king_piece && king_piece->is_legal_move(new_pos)) {
                         if (!would_be_in_check_after_move(king_pos, new_pos)) {
                            return false; // King can move out of check
                        }
                    }
                }
            }
        }
    }

    // 2. Can any piece block the check or capture the attacker?
    std::vector<position> attackers;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (pieces[i][j] != nullptr && pieces[i][j]->get_clr() != current_turn)
            {
                turn original_turn = current_turn;
                current_turn = pieces[i][j]->get_clr();
                if (pieces[i][j]->is_legal_move(king_pos))
                {
                    attackers.push_back(position(i, j));
                }
                current_turn = original_turn;
            }
        }
    }

    if (attackers.empty()) { // Should not happen if is_check() is true
        return false;
    }

    // If there are multiple attackers, only a king move can resolve it (handled above)
    if (attackers.size() > 1) {
        return true;
    }

    position attacker_pos = attackers[0];

    // Try to capture the attacker
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (pieces[i][j] != nullptr && pieces[i][j]->get_clr() == current_turn)
            {
                if (is_legal(position(i,j), attacker_pos) && !would_be_in_check_after_move(position(i,j), attacker_pos))
                {
                    return false; // Attacker can be captured
                }
            }
        }
    }

    // Try to block the check (only for sliding pieces)
    piece* attacker_piece = pieces[attacker_pos.x][attacker_pos.y];
    bool is_sliding_piece = (dynamic_cast<rook*>(attacker_piece) != nullptr || 
                             dynamic_cast<bishop*>(attacker_piece) != nullptr ||
                             dynamic_cast<queen*>(attacker_piece) != nullptr);

    if (is_sliding_piece)
    {
        std::vector<position> path_to_king;
        int dx = (king_pos.x == attacker_pos.x) ? 0 : (king_pos.x > attacker_pos.x ? 1 : -1);
        int dy = (king_pos.y == attacker_pos.y) ? 0 : (king_pos.y > attacker_pos.y ? 1 : -1);

        for (int x = attacker_pos.x + dx, y = attacker_pos.y + dy;
             x != king_pos.x || y != king_pos.y;
             x += dx, y += dy)
        {
            path_to_king.push_back(position(x, y));
        }

        for (position block_pos : path_to_king)
        {
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (pieces[i][j] != nullptr && pieces[i][j]->get_clr() == current_turn)
                    {
                        if (is_legal(position(i,j), block_pos) && !would_be_in_check_after_move(position(i,j), block_pos))
                        {
                            return false; // Check can be blocked
                        }
                    }
                }
            }
        }
    }
    
    return true; // No way to escape check
}

bool board::is_legal(position s_pos, position d_pos)
{
    if (!is_valid_source(s_pos) || !is_valid_destination(d_pos))
        return false;

    piece* p = get_piece(s_pos);
    if (!p) return false;

    if (p->get_type() == 5) { // King
        if (s_pos.x == d_pos.x && std::abs(s_pos.y - d_pos.y) == 2) { // Castling
            bool kingside = (d_pos.y > s_pos.y);
            return kingside ? can_castle_kingside(current_turn) : can_castle_queenside(current_turn);
        }
    }
    return p->is_legal_move(d_pos);
}

void board::move_piece(position s_pos, position d_pos)
{
    piece* p = get_piece(s_pos);
    if (!p) return;

    if (p->get_type() == 5) // King
    {
        if (std::abs(s_pos.y - d_pos.y) == 2) // Castling
        {
            bool kingside = (d_pos.y > s_pos.y);
            perform_castle(kingside);
            // After castling, mark king and rook as moved
            piece_moved[d_pos.x][d_pos.y] = true; // New king position
            if (kingside) {
                piece_moved[d_pos.x][d_pos.y - 1] = true; // New rook position
            } else {
                piece_moved[d_pos.x][d_pos.y + 1] = true; // New rook position
            }
            return;
        }
    }

    // Handle en passant capture
    if (p->get_type() == 0) { // Pawn
        if (d_pos == en_passant_target) {
            // Captured pawn is behind d_pos
            if (p->get_clr() == white) {
                delete pieces[d_pos.x + 1][d_pos.y];
                pieces[d_pos.x + 1][d_pos.y] = nullptr;
            } else {
                delete pieces[d_pos.x - 1][d_pos.y];
                pieces[d_pos.x - 1][d_pos.y] = nullptr;
            }
        }
    }

    // Delete captured piece if any
    if (pieces[d_pos.x][d_pos.y] != nullptr)
    {
        delete pieces[d_pos.x][d_pos.y];
        pieces[d_pos.x][d_pos.y] = nullptr;
    }

    pieces[s_pos.x][s_pos.y]->move(d_pos);
    pieces[d_pos.x][d_pos.y] = pieces[s_pos.x][s_pos.y];
    pieces[s_pos.x][s_pos.y] = nullptr;

    piece_moved[d_pos.x][d_pos.y] = true;

    // Set en passant target for next turn if pawn moved two squares
    en_passant_target = position(-1, -1); // Reset
    if (p->get_type() == 0) { // Pawn
        if (std::abs(s_pos.x - d_pos.x) == 2) {
            en_passant_target = position((s_pos.x + d_pos.x) / 2, s_pos.y);
        }
    }
}

void board::change_turn()
{
    current_turn = (current_turn == white) ? black : white;
}

bool board::would_be_in_check_after_move(position s_pos, position d_pos) {
    // Simulate move
    piece* s_piece = pieces[s_pos.x][s_pos.y];
    if (!s_piece) return false;

    piece* d_piece_original = pieces[d_pos.x][d_pos.y];
    position s_piece_original_pos = s_piece->get_pos();

    // Perform move
    pieces[d_pos.x][d_pos.y] = s_piece;
    pieces[s_pos.x][s_pos.y] = nullptr;
    s_piece->move(d_pos);

    // Check if in check
    bool would_be_in_check = is_check(); // Use the is_check logic

    // Undo move
    pieces[s_pos.x][s_pos.y] = s_piece;
    pieces[d_pos.x][d_pos.y] = d_piece_original;
    s_piece->move(s_piece_original_pos);

    return would_be_in_check;
}

void board::promote_pawn(position pos, int promotion_type)
{
    piece* p = get_piece(pos);
    if (!p || p->get_type() != 0) return; 

    turn piece_color = p->get_clr();

    // Delete the old pawn
    delete pieces[pos.x][pos.y];
    pieces[pos.x][pos.y] = nullptr;

    switch (promotion_type)
    {
    case 1: // Queen
        pieces[pos.x][pos.y] = new queen(pos, this, piece_color);
        break;
    case 2: // Rook
        pieces[pos.x][pos.y] = new rook(pos, this, piece_color);
        break;
    case 3: // Bishop
        pieces[pos.x][pos.y] = new bishop(pos, this, piece_color);
        break;
    case 4: // Knight
        pieces[pos.x][pos.y] = new knight(pos, this, piece_color);
        break;
    default:
        // Invalid promotion type, promote to Queen as default
        pieces[pos.x][pos.y] = new queen(pos, this, piece_color);
        break;
    }
}

bool board::piece_has_moved(position pos)
{
    if (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7) return true; // Treat invalid positions as moved
    return piece_moved[pos.x][pos.y];
}

bool board::can_castle_kingside(turn color) {
    int rank = (color == white) ? 7 : 0;
    position king_pos(rank, 4);
    position rook_pos(rank, 7);

    piece* king_piece = get_piece(king_pos);
    piece* rook_piece = get_piece(rook_pos);

    if (!king_piece || king_piece->get_type() != 5 || king_piece->get_clr() != color) return false;
    if (!rook_piece || rook_piece->get_type() != 1 || rook_piece->get_clr() != color) return false; // Fixed type to 1 for rook

    if (piece_moved[rank][4] || piece_moved[rank][7]) return false;

    if (get_piece(position(rank, 5)) || get_piece(position(rank, 6))) return false;

    // Check if king is currently in check
    if (is_check()) return false;
    
    // Check if squares king passes through or lands on are attacked
    if (would_be_in_check_after_move(king_pos, position(rank, 5))) return false;
    if (would_be_in_check_after_move(king_pos, position(rank, 6))) return false;

    return true;
}
    
bool board::can_castle_queenside(turn color) {
    int rank = (color == white) ? 7 : 0;
    position king_pos(rank, 4);
    position rook_pos(rank, 0);

    piece* king_piece = get_piece(king_pos);
    piece* rook_piece = get_piece(rook_pos);

    if (!king_piece || king_piece->get_type() != 5 || king_piece->get_clr() != color) return false;
    if (!rook_piece || rook_piece->get_type() != 1 || rook_piece->get_clr() != color) return false; // Fixed type to 1 for rook

    if (piece_moved[king_pos.x][king_pos.y] || piece_moved[rook_pos.x][rook_pos.y]) {
        return false;
    }
    // Check if squares between king and rook are empty
    for (int y = rook_pos.y + 1; y < king_pos.y; y++) {
        if (get_piece(position(rank, y)) != nullptr) {
            return false;
        }
    }

    // Check if king is currently in check
    if (is_check()) {
        return false;
    }

    // Check if squares king passes through or lands on are attacked
    if (would_be_in_check_after_move(king_pos, position(rank, king_pos.y - 1)) ||
        would_be_in_check_after_move(king_pos, position(rank, king_pos.y - 2))) {
        return false;
    }

    return true;
}

bool board::is_stalemate()
{
    if (is_check()) return false;

    // Iterate through all current_turn's pieces and check for any legal moves
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            position src(i, j);
            piece* p = get_piece(src);

            if (!p || p->get_clr() != get_current_turn()) continue;

            for (int di = 0; di < 8; di++) {
                for (int dj = 0; dj < 8; dj++) {
                    position dest(di, dj);

                    // If a legal move is found that does not result in self-check, it's not a stalemate
                    if (is_legal(src, dest) && !would_be_in_check_after_move(src, dest)) {
                        return false; 
                    }
                }
            }
        }
    }

    // If we get here, the player has no legal moves and is not in check
    return true;
}

void board::perform_castle(bool kingside) {
    int rank = (current_turn == white) ? 7 : 0;
    position king_pos(rank, 4);

    if (kingside) {
        // King moves two squares to the right
        position rook_pos(rank, 7);
        position new_king_pos(rank, 6);
        position new_rook_pos(rank, 5);

        // Move king
        pieces[new_king_pos.x][new_king_pos.y] = pieces[king_pos.x][king_pos.y];
        pieces[king_pos.x][king_pos.y] = nullptr;
        pieces[new_king_pos.x][new_king_pos.y]->move(new_king_pos);

        // Move rook
        pieces[new_rook_pos.x][new_rook_pos.y] = pieces[rook_pos.x][rook_pos.y];
        pieces[rook_pos.x][rook_pos.y] = nullptr;
        pieces[new_rook_pos.x][new_rook_pos.y]->move(new_rook_pos);

        // Mark pieces as moved (handled in move_piece or callers if this is standalone)
        // For safety, explicitly mark them here as well
        piece_moved[new_king_pos.x][new_king_pos.y] = true;
        piece_moved[new_rook_pos.x][new_rook_pos.y] = true;
    }
    else { // Queenside
        // King moves two squares to the left
        position rook_pos(rank, 0);
        position new_king_pos(rank, 2);
        position new_rook_pos(rank, 3);

        // Move king
        pieces[new_king_pos.x][new_king_pos.y] = pieces[king_pos.x][king_pos.y];
        pieces[king_pos.x][king_pos.y] = nullptr;
        pieces[new_king_pos.x][new_king_pos.y]->move(new_king_pos);

        // Move rook
        pieces[new_rook_pos.x][new_rook_pos.y] = pieces[rook_pos.x][rook_pos.y];
        pieces[rook_pos.x][rook_pos.y] = nullptr;
        pieces[new_rook_pos.x][new_rook_pos.y]->move(new_rook_pos);

        // Mark pieces as moved
        piece_moved[new_king_pos.x][new_king_pos.y] = true;
        piece_moved[new_rook_pos.x][new_rook_pos.y] = true;
    }
}

bool board::has_king_moved(int color)
{
    int rank = (color == white) ? 7 : 0;
    position king_pos(rank, 4); 
    if (king_pos.x < 0 || king_pos.x > 7 || king_pos.y < 0 || king_pos.y > 7) return true; // Invalid pos, assume moved
    return piece_moved[king_pos.x][king_pos.y];
}

bool board::has_rook_moved(int color, bool kingside) {
    int rank = (color == white) ? 7 : 0;
    int file = kingside ? 7 : 0; 
    position rook_pos(rank, file);
    if (rook_pos.x < 0 || rook_pos.x > 7 || rook_pos.y < 0 || rook_pos.y > 7) return true; // Invalid pos, assume moved
    return piece_moved[rook_pos.x][rook_pos.y];
}

position board::get_en_passant_target()
{
    return en_passant_target;
}

void board::set_king_moved(int color, bool moved)
{
    int rank = (color == white) ? 7 : 0;
    position king_pos(rank, 4);
    if (king_pos.x >= 0 && king_pos.x < 8 && king_pos.y >= 0 && king_pos.y < 8) {
        piece_moved[king_pos.x][king_pos.y] = moved;
    }
}

void board::set_rook_moved(int color, bool kingside, bool moved)
{
    int rank = (color == white) ? 7 : 0;
    int file = kingside ? 7 : 0;
    position rook_pos(rank, file);
    if (rook_pos.x >= 0 && rook_pos.x < 8 && rook_pos.y >= 0 && rook_pos.y < 8) {
        piece_moved[rook_pos.x][rook_pos.y] = moved;
    }
}

void board::set_en_passant_target(position target)
{
    en_passant_target = target;
}

void board::place_piece(position to, int type, int color)
{
    if (to.x < 0 || to.x > 7 || to.y < 0 || to.y > 7) return; // Invalid position

    if (pieces[to.x][to.y] != nullptr) 
    {
        delete pieces[to.x][to.y];
        pieces[to.x][to.y] = nullptr;
    }

    turn piece_color = (color == 0) ? white : black;

    switch (type) 
    {
    case 0: // Pawn
        pieces[to.x][to.y] = new pawn(to, this, piece_color);
        break;
    case 1: // Rook (was bishop type 1, changed to 1 based on previous code)
        pieces[to.x][to.y] = new rook(to, this, piece_color);
        break;
    case 2: // Knight
        pieces[to.x][to.y] = new knight(to, this, piece_color);
        break;
    case 3: // Bishop (was knight type 3, changed to 3 based on previous code)
        pieces[to.x][to.y] = new bishop(to, this, piece_color);
        break;
    case 4: // Queen
        pieces[to.x][to.y] = new queen(to, this, piece_color);
        break;
    case 5: // King
        pieces[to.x][to.y] = new king(to, this, piece_color);
        break;
    default:
        // Invalid piece type, do nothing
        break;
    }
}

void board::remove_piece(position p) {
    if (p.x < 0 || p.x > 7 || p.y < 0 || p.y > 7) return;
    if (pieces[p.x][p.y] != nullptr) {
        delete pieces[p.x][p.y];
        pieces[p.x][p.y] = nullptr;
    }
}

void board::set_piece_at(position p, piece* p_ptr) {
    if (p.x < 0 || p.x > 7 || p.y < 0 || p.y > 7) return;
    // The previous piece at p.x, p.y should be handled by the caller or be null
    pieces[p.x][p.y] = p_ptr;
}