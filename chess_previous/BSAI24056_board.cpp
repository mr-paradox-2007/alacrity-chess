#include "BSAI24056_board.h"

struct BoardPalette {
    Color light;
    Color dark;
    Color border;
    Color highlight;
    Color borderTop;
    const char* name;
};
const BoardPalette BOARD_PALETTES[] = {
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
}

void board::reset(bool reseted)
{
    init_pieces(reseted);
}

void board::print_board()
{
    const BoardPalette& palette = BOARD_PALETTES[active_palette_index];

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
                DrawCube(border_top_pos, sq, 0.01f, sq, palette.borderTop);
            }
        }
    }

    for (int x = 0; x < N; x++) 
    {
        for (int z = 0; z < N; z++) 
        {
            Vector3 pos = { off + x * sq, board_height / 2.0f, off + z * sq };
            bool is_light = ((x + z) % 2) == 0;
            Color squareColor = is_light ? palette.light : palette.dark;

            DrawCube(pos, sq, board_height, sq, squareColor);

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
    int num_palettes = sizeof(BOARD_PALETTES) / sizeof(BOARD_PALETTES[0]);
    active_palette_index = index % num_palettes;
    if (active_palette_index < 0) active_palette_index = num_palettes - 1;
}

void board::cycle_palette()
{
    int num_palettes = sizeof(BOARD_PALETTES) / sizeof(BOARD_PALETTES[0]);
    active_palette_index = (active_palette_index + 1) % num_palettes;
}

piece* board::get_piece(position _pos)
{
    return pieces[_pos.x][_pos.y];
}

void board::init_pieces(bool reseted)
{
    
    if (reseted)
    {
        for (int x = 0; x < 8; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                pieces[x][y] = nullptr;
            }
        }
    }

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (pieces[x][y] != nullptr)
            {
                delete pieces[x][y];
            }
            pieces[x][y] = nullptr;
        }
    }

    // White pawns
    for (int y = 0; y < 8; y++)
    {
        if (pieces[6][y] != nullptr)
        {
            delete pieces[6][y];
        }
        pieces[6][y] = new pawn({ 6, y }, this, white);
    }

    // Black pawns
    for (int y = 0; y < 8; y++)
    {
        if (pieces[1][y] != nullptr)
        {
            delete pieces[1][y];
        }
        pieces[1][y] = new pawn({ 1, y }, this, black);
    }

    // Black back‐rank
    if (pieces[0][0] != nullptr) delete pieces[0][0];
    pieces[0][0] = new rook({ 0, 0 }, this, black);

    if (pieces[0][1] != nullptr) delete pieces[0][1];
    pieces[0][1] = new knight({ 0, 1 }, this, black);

    if (pieces[0][2] != nullptr) delete pieces[0][2];
    pieces[0][2] = new bishop({ 0, 2 }, this, black);

    if (pieces[0][3] != nullptr) delete pieces[0][3];
    pieces[0][3] = new queen({ 0, 3 }, this, black);

    if (pieces[0][4] != nullptr) delete pieces[0][4];
    pieces[0][4] = new king({ 0, 4 }, this, black);

    if (pieces[0][5] != nullptr) delete pieces[0][5];
    pieces[0][5] = new bishop({ 0, 5 }, this, black);

    if (pieces[0][6] != nullptr) delete pieces[0][6];
    pieces[0][6] = new knight({ 0, 6 }, this, black);

    if (pieces[0][7] != nullptr) delete pieces[0][7];
    pieces[0][7] = new rook({ 0, 7 }, this, black);

    // White back‐rank
    if (pieces[7][0] != nullptr) delete pieces[7][0];
    pieces[7][0] = new rook({ 7, 0 }, this, white);

    if (pieces[7][1] != nullptr) delete pieces[7][1];
    pieces[7][1] = new knight({ 7, 1 }, this, white);

    if (pieces[7][2] != nullptr) delete pieces[7][2];
    pieces[7][2] = new bishop({ 7, 2 }, this, white);

    if (pieces[7][3] != nullptr) delete pieces[7][3];
    pieces[7][3] = new queen({ 7, 3 }, this, white);

    if (pieces[7][4] != nullptr) delete pieces[7][4];
    pieces[7][4] = new king({ 7, 4 }, this, white);

    if (pieces[7][5] != nullptr) delete pieces[7][5];
    pieces[7][5] = new bishop({ 7, 5 }, this, white);

    if (pieces[7][6] != nullptr) delete pieces[7][6];
    pieces[7][6] = new knight({ 7, 6 }, this, white);

    if (pieces[7][7] != nullptr) delete pieces[7][7];
    pieces[7][7] = new rook({ 7, 7 }, this, white);
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
    if (_pos.x < 0 || _pos.x>7 || _pos.y < 0 || _pos.y>7)
        return false;
    if (pieces[_pos.x][_pos.y] == nullptr)
        return false;

    return pieces[_pos.x][_pos.y]->get_clr() == current_turn;
}
bool board::is_valid_destination(position _pos)
{
    if (_pos.x < 0 || _pos.x>7 || _pos.y < 0 || _pos.y>7)
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
    return position(-1, -1);
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
                if (pieces[i][j]->is_legal_move(king_pos))
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

    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0)
                continue;

            position new_pos(king_pos.x + dx, king_pos.y + dy);

            if (new_pos.x >= 0 && new_pos.x < 8 && new_pos.y >= 0 && new_pos.y < 8)
            {
                if (is_valid_destination(new_pos) && pieces[king_pos.x][king_pos.y]->is_legal_move(new_pos))
                {
                    piece* temp = pieces[new_pos.x][new_pos.y];
                    pieces[new_pos.x][new_pos.y] = pieces[king_pos.x][king_pos.y];
                    pieces[king_pos.x][king_pos.y] = nullptr;

                    position old_pos = pieces[new_pos.x][new_pos.y]->get_pos();
                    pieces[new_pos.x][new_pos.y]->move(new_pos);

                    bool still_in_check = false;
                    for (int i = 0; i < 8 && !still_in_check; i++)
                    {
                        for (int j = 0; j < 8 && !still_in_check; j++)
                        {
                            if (pieces[i][j] != nullptr && pieces[i][j]->get_clr() != current_turn)
                            {
                                if (pieces[i][j]->is_legal_move(new_pos))
                                {
                                    still_in_check = true;
                                }
                            }
                        }
                    }

                    pieces[king_pos.x][king_pos.y] = pieces[new_pos.x][new_pos.y];
                    pieces[new_pos.x][new_pos.y] = temp;
                    pieces[king_pos.x][king_pos.y]->move(old_pos);

                    if (!still_in_check)
                        return false;
                }
            }
        }
    }

    vector<position> attackers;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (pieces[i][j] != nullptr && pieces[i][j]->get_clr() != current_turn)
            {
                if (pieces[i][j]->is_legal_move(king_pos))
                {
                    attackers.push_back(position(i, j));
                }
            }
        }
    }

    if (attackers.size() > 1)
        return true;

    position attacker = attackers[0];

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (pieces[i][j] != nullptr && pieces[i][j]->get_clr() == current_turn &&
                dynamic_cast<king*>(pieces[i][j]) == nullptr)
            {
                if (pieces[i][j]->is_legal_move(attacker))
                {
                    piece* temp = pieces[attacker.x][attacker.y];
                    position old_pos = pieces[i][j]->get_pos();
                    pieces[attacker.x][attacker.y] = pieces[i][j];
                    pieces[i][j] = nullptr;
                    pieces[attacker.x][attacker.y]->move(attacker);

                    bool still_in_check = false;
                    for (int x = 0; x < 8 && !still_in_check; x++)
                    {
                        for (int y = 0; y < 8 && !still_in_check; y++)
                        {
                            if (pieces[x][y] != nullptr && pieces[x][y]->get_clr() != current_turn)
                            {
                                if (pieces[x][y]->is_legal_move(king_pos))
                                {
                                    still_in_check = true;
                                }
                            }
                        }
                    }

                    pieces[i][j] = pieces[attacker.x][attacker.y];
                    pieces[attacker.x][attacker.y] = temp;
                    pieces[i][j]->move(old_pos);

                    if (!still_in_check)
                        return false;
                }
            }
        }
    }

    if (dynamic_cast<bishop*>(pieces[attacker.x][attacker.y]) != nullptr ||
        dynamic_cast<rook*>(pieces[attacker.x][attacker.y]) != nullptr ||
        dynamic_cast<queen*>(pieces[attacker.x][attacker.y]) != nullptr)
    {
        vector<position> path;
        int dx = king_pos.x - attacker.x;
        int dy = king_pos.y - attacker.y;
        int steps = max(abs(dx), abs(dy));
        dx = (dx != 0) ? dx / abs(dx) : 0;
        dy = (dy != 0) ? dy / abs(dy) : 0;

        for (int i = 1; i < steps; i++)
        {
            path.push_back(position(attacker.x + i * dx, attacker.y + i * dy));
        }

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (pieces[i][j] != nullptr && pieces[i][j]->get_clr() == current_turn &&
                    dynamic_cast<king*>(pieces[i][j]) == nullptr)
                {
                    for (position block_pos : path)
                    {
                        if (pieces[i][j]->is_legal_move(block_pos))
                        {
                            position old_pos = pieces[i][j]->get_pos();
                            pieces[block_pos.x][block_pos.y] = pieces[i][j];
                            pieces[i][j] = nullptr;
                            pieces[block_pos.x][block_pos.y]->move(block_pos);

                            bool still_in_check = false;
                            for (int x = 0; x < 8 && !still_in_check; x++)
                            {
                                for (int y = 0; y < 8 && !still_in_check; y++)
                                {
                                    if (pieces[x][y] != nullptr && pieces[x][y]->get_clr() != current_turn)
                                    {
                                        if (pieces[x][y]->is_legal_move(king_pos))
                                        {
                                            still_in_check = true;
                                        }
                                    }
                                }
                            }

                            pieces[i][j] = pieces[block_pos.x][block_pos.y];
                            pieces[block_pos.x][block_pos.y] = nullptr;
                            pieces[i][j]->move(old_pos);

                            if (!still_in_check)
                                return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}
bool board::is_legal(position s_pos, position d_pos)
{
    if (!is_valid_source(s_pos) || !is_valid_destination(d_pos))
        return false;

    piece* p = get_piece(s_pos);
    if (p && p->get_type() == 5) { 
        if (s_pos.x == d_pos.x && abs(s_pos.y - d_pos.y) == 2) {
            bool kingside = (d_pos.y > s_pos.y);
            return kingside ? can_castle_kingside(current_turn) : can_castle_queenside(current_turn);
        }
    }

    return pieces[s_pos.x][s_pos.y]->is_legal_move(d_pos);
}
void board::move_piece(position s_pos, position d_pos)
{
    piece* p = get_piece(s_pos);
    if (p && p->get_type() == 5)
    {
        if (abs(s_pos.y - d_pos.y) == 2)
        {
            bool kingside = (d_pos.y > s_pos.y);
            perform_castle(kingside);
            return;
        }
    }

    pieces[s_pos.x][s_pos.y]->move(d_pos);
    pieces[d_pos.x][d_pos.y] = pieces[s_pos.x][s_pos.y];
    pieces[s_pos.x][s_pos.y] = nullptr;

    piece_moved[d_pos.x][d_pos.y] = true;
}
void board::change_turn()
{
    current_turn = current_turn == white ? black : white;
}
bool board::would_be_in_check_after_move(position s_pos, position d_pos) {
    piece* s_piece = pieces[s_pos.x][s_pos.y];
    piece* d_piece = pieces[d_pos.x][d_pos.y];
    position old_pos = s_piece ? s_piece->get_pos() : position(-1, -1);

    if (s_piece) {
        pieces[d_pos.x][d_pos.y] = s_piece;
        pieces[s_pos.x][s_pos.y] = nullptr;
        s_piece->move(d_pos);
    }

    position king_pos = find_king();
    bool would_be_in_check = false;

    for (int i = 0; i < 8 && !would_be_in_check; i++) {
        for (int j = 0; j < 8 && !would_be_in_check; j++) {
            if (pieces[i][j] && pieces[i][j]->get_clr() != current_turn) {
                if (pieces[i][j]->is_legal_move(king_pos)) {
                    would_be_in_check = true;
                }
            }
        }
    }

    pieces[s_pos.x][s_pos.y] = s_piece;
    pieces[d_pos.x][d_pos.y] = d_piece;
    if (s_piece) s_piece->move(old_pos);

    return would_be_in_check;
}
std::vector<position> highlighted_positions;

void board::promote_pawn(position pos, int promotion_type)
{
    piece* p = get_piece(pos);
    if (!p || p->get_type() != 0) return; 

    turn piece_color = p->get_clr();

    pieces[pos.x][pos.y] = nullptr;

    switch (promotion_type)
    {
    case 1: 
        pieces[pos.x][pos.y] = new queen(pos, this, piece_color);
        break;
    case 2: 
        pieces[pos.x][pos.y] = new rook(pos, this, piece_color);
        break;
    case 3: 
        pieces[pos.x][pos.y] = new bishop(pos, this, piece_color);
        break;
    case 4: 
        pieces[pos.x][pos.y] = new knight(pos, this, piece_color);
        break;
    }
}

bool board::piece_has_moved(position pos)
{
    return piece_moved[pos.x][pos.y];
}

bool board::can_castle_kingside(turn color) {
    int rank = (color == white) ? 7 : 0;
    position king_pos(rank, 4);
    position rook_pos(rank, 7);

    piece* king = get_piece(king_pos);
    piece* rook = get_piece(rook_pos);

    if (!king || king->get_type() != 5 || king->get_clr() != color) return false;
    if (!rook || rook->get_type() != 2 || rook->get_clr() != color) return false;

    if (piece_moved[rank][4] || piece_moved[rank][7]) return false;

    if (get_piece(position(rank, 5)) || get_piece(position(rank, 6))) return false;

    if (is_check()) return false;
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

    if (!king_piece || king_piece->get_type() != 5 || king_piece->get_clr() != color ||
        !rook_piece || rook_piece->get_type() != 2 || rook_piece->get_clr() != color) {
        return false;
    }

    if (piece_moved[king_pos.x][king_pos.y] || piece_moved[rook_pos.x][rook_pos.y]) {
        return false;
    }
    for (int y = rook_pos.y + 1; y < king_pos.y; y++) {
        if (get_piece(position(rank, y)) != nullptr) {
            return false;
        }
    }

    if (is_check()) {
        return false;
    }

    if (would_be_in_check_after_move(king_pos, position(rank, king_pos.y - 1)) ||
        would_be_in_check_after_move(king_pos, position(rank, king_pos.y - 2))) {
        return false;
    }

    return true;
}


bool board::is_stalemate()
{
    if (is_check()) return false;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            position src(i, j);
            piece* p = get_piece(src);

            if (!p || p->get_clr() != get_current_turn()) continue;

            for (int di = 0; di < 8; di++) {
                for (int dj = 0; dj < 8; dj++) {
                    position dest(di, dj);

                    if (is_legal(src, dest)) {
                        if (src.x == dest.x && src.y == dest.y) continue;

                        if (p->get_type() == 5 && abs(src.y - dest.y) == 2) {
                            bool kingside = (dest.y > src.y);
                            if (kingside ? can_castle_kingside(current_turn) : can_castle_queenside(current_turn)) {
                                return false;
                            }
                            continue; 
                        }

                        // For non-castling moves, ensure move doesn't put king in check
                        if (!would_be_in_check_after_move(src, dest)) {
                            return false; // Found a legal move
                        }
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

        // Mark pieces as moved
        piece_moved[new_king_pos.x][new_king_pos.y] = true;
        piece_moved[new_rook_pos.x][new_rook_pos.y] = true;
    }
    else {
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
    return piece_moved[king_pos.x][king_pos.y];
}

bool board::has_rook_moved(int color, bool kingside) {
    int rank = (color == white) ? 7 : 0;
    int file = kingside ? 7 : 0; 
    position rook_pos(rank, file);
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
    piece_moved[king_pos.x][king_pos.y] = moved;
}

void board::set_rook_moved(int color, bool kingside, bool moved)
{
    int rank = (color == white) ? 7 : 0;
    int file = kingside ? 7 : 0;
    position rook_pos(rank, file);
    piece_moved[rook_pos.x][rook_pos.y] = moved;
}

void board::set_en_passant_target(position target)
{
    en_passant_target = target;
}


void board::place_piece(position to, int type, int color)
{
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
    case 1: // Bishop
        pieces[to.x][to.y] = new bishop(to, this, piece_color);
        break;
    case 2: // Rook
        pieces[to.x][to.y] = new rook(to, this, piece_color);
        break;
    case 3: // Knight
        pieces[to.x][to.y] = new knight(to, this, piece_color);
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