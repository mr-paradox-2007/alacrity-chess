#include "game_player.hpp"
#include <vector>
#include <cmath> // For std::fabs, std::roundf
#include <cstdio> // For snprintf
#include <iostream> // For std::cout (temporarily for SAVED GAME)

game_player::game_player() :
    t(white),
    saved_turn(white),
    selected_pos(-1, -1),
    pos(-1, -1),
    promotion_position(-1, -1),
    is_paused(false),
    piece_selected(false),
    promotion_pending(false),
    quit(false),
    replay_mode(false)
{
    // Initialize board and camera here if needed, or in reset()
    reset(false); // Initial reset
}


position game_player::get_board_position_from_mouse()
{
    input.update_mouse();
    Ray ray = GetMouseRay(input.get_m(), cam.get_camera());
    const float board_height = 0.45f;
    const float plane_y = board_height / 2.0f;
    const float sq = 1.0f;
    const float off = -3.5f * sq;

    Vector3 n = { 0, 1, 0 }, p0 = { 0, plane_y, 0 };
    float denom = Vector3DotProduct(ray.direction, n);

    if (std::fabs(denom) > 1e-6f)
    {
        Vector3 diff = Vector3Subtract(p0, ray.position);
        float t_val = Vector3DotProduct(diff, n) / denom; // Renamed 't' to 't_val' to avoid conflict with 'this->t'

        if (t_val >= 0)
        {
            Vector3 hit_point = Vector3Add(ray.position, Vector3Scale(ray.direction, t_val));

            int file = (int)std::roundf((hit_point.x - off) / sq);
            int rank = (int)std::roundf((hit_point.z - off) / sq);

            return position(rank, file);
        }
    }

    return position(-1, -1);
}

void game_player::handle_piece_selection(position pos_clicked) // Renamed pos to pos_clicked
{
    if (b.is_valid_source(pos_clicked))
    {
        piece* p = b.get_piece(pos_clicked);
        if (p && p->get_clr() == t)
        {
            piece_selected = true;
            selected_pos = pos_clicked;

            valid_moves.clear();
            calculate_valid_moves();
        }
    }
}

void game_player::calculate_valid_moves()
{
    piece* p = b.get_piece(selected_pos);
    if (!p)
        return;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position dest_pos(i, j);

            if (b.is_valid_destination(dest_pos) && p->is_legal_move(dest_pos) &&
                !b.would_be_in_check_after_move(selected_pos, dest_pos))
            {
                valid_moves.push_back(dest_pos);
            }
        }
    }

    if (p->get_type() == 5) // King
    {
        if (b.can_castle_kingside(p->get_clr()))
        {
            position castle_pos(selected_pos.x, selected_pos.y + 2);
            valid_moves.push_back(castle_pos);
        }
        if (b.can_castle_queenside(p->get_clr()))
        {
            position castle_pos(selected_pos.x, selected_pos.y - 2);
            valid_moves.push_back(castle_pos);
        }
    }
}

void game_player::handle_piece_move(position pos_clicked, audio_manager& audio) // Renamed pos to pos_clicked
{
    if (replay_mode) return;

    if (pos_clicked == selected_pos) {
        piece_selected = false;
        valid_moves.clear();
        return;
    }

    if (b.is_valid_destination(pos_clicked))
    {
        piece* p = b.get_piece(selected_pos);
        if (!p) return; // Should not happen if piece_selected is true

        if (b.get_piece(pos_clicked) && b.get_piece(pos_clicked)->get_clr() == t)
        {
            selected_pos = pos_clicked;
            valid_moves.clear();
            if (b.is_valid_destination(selected_pos)) // This condition seems redundant here
                calculate_valid_moves();
            return;
        }

        bool is_castling = false;
        if (p->get_type() == 5) // King
        {
            if (selected_pos.x == pos_clicked.x && std::abs(selected_pos.y - pos_clicked.y) == 2)
            {
                is_castling = true;
            }
        }

        bool move_is_legal = false;
        for (const auto& valid_move : valid_moves) {
            if (valid_move == pos_clicked) {
                move_is_legal = true;
                break;
            }
        }

        if (move_is_legal)
        {
            if (p->get_type() == 0) // Pawn
            {
                // Check for pawn promotion
                if ((p->get_clr() == white && pos_clicked.x == 0) ||
                    (p->get_clr() == black && pos_clicked.x == 7))
                {
                    promotion_pending = true;
                    promotion_position = pos_clicked;
                    return; // Wait for promotion UI input
                }
                // Check for en passant
                if (pos_clicked == b.get_en_passant_target() && pos_clicked.y != selected_pos.y) {
                    chess_move move = create_move_record(selected_pos, pos_clicked);
                    move.was_en_passant = true;
                    b.move_piece(selected_pos, pos_clicked);
                    history.add_move(move);
                } else {
                    chess_move move = create_move_record(selected_pos, pos_clicked);
                    b.move_piece(selected_pos, pos_clicked);
                    history.add_move(move);
                }
            } else {
                chess_move move = create_move_record(selected_pos, pos_clicked);
                b.move_piece(selected_pos, pos_clicked);
                history.add_move(move);
            }
            
            if (t == white)
                audio.play_move_white();
            else
                audio.play_move_black();

            t = (t == white ? black : white);
            b.change_turn();
            cam.flip_camera();

            if (b.is_check())
            {
                audio.play_check();
            }

            piece_selected = false;
            valid_moves.clear();
        } else {
            // Invalid move, deselect piece
            piece_selected = false;
            valid_moves.clear();
        }
    }


    if (b.is_checkmate() || b.is_stalemate()) {
        // Game over, handled in play() loop
    }
}

void game_player::draw_board_highlights()
{
    const float board_height = 0.45f;
    const float highlight_height = board_height / 2.0f + 0.01f;
    const float sq = 1.0f;
    const float off = -3.5f * sq;

    position hover_pos = get_board_position_from_mouse();

    if (piece_selected)
    {
        Vector3 sel = { off + selected_pos.y * sq,
                        highlight_height,
                        off + selected_pos.x * sq };
        DrawCube(sel, sq, 0.5f, sq, YELLOW);
    }

    for (auto& mv : valid_moves)
    {
        Color col = (b.get_piece(mv) != nullptr) ? RED : GREEN;
        Vector3 m = { off + mv.y * sq,
                      highlight_height,
                      off + mv.x * sq };
        DrawCube(m, sq, 0.5f, sq, ColorAlpha(col, 0.47f));
    }

    piece* selP = b.get_piece(selected_pos);
    if (piece_selected && selP && selP->get_type() == 5) // King selected
    {
        for (auto& mv : valid_moves)
        {
            if (mv.x == selected_pos.x && std::abs(mv.y - selected_pos.y) == 2) // Castling move
            {
                bool kingside = mv.y > selected_pos.y;
                int rank = selected_pos.x;
                // Rook's original position (from) and final position (to)
                position rook_from = kingside ? position(rank, 7) : position(rank, 0);
                position rook_to = kingside ? position(rank, 5) : position(rank, 3);

                Vector3 rf = { off + rook_from.y * sq, highlight_height, off + rook_from.x * sq };
                Vector3 rt = { off + rook_to.y * sq, highlight_height, off + rook_to.x * sq };

                Color orange_alpha = ColorAlpha(Color{ 255,165,0,255 }, 0.5f);
                DrawCube(rf, sq, 0.5f, sq, orange_alpha);
                DrawCube(rt, sq, 0.5f, sq, orange_alpha);
            }
        }
    }

    if (hover_pos.x >= 0 && hover_pos.x < 8 && hover_pos.y >= 0 && hover_pos.y < 8)
    {
        piece* p = b.get_piece(hover_pos);
        if (p && p->get_clr() == t)
        {
            Vector3 h = { off + hover_pos.y * sq, highlight_height, off + hover_pos.x * sq };
            DrawCube(h, sq, 0.5f, sq, ColorAlpha(SKYBLUE, 0.7f));
        }
    }
}

void game_player::game_paused()
{
    if (!is_paused)
        return;

    DrawRectangle(0, 0, internal_width, internal_height, Color{ 0, 0, 0, 120 });

    static Font pause_font = LoadFont("assets/Montserrat-ExtraBold.ttf");
    static int font_size = 50;

    float button_width = 400;
    float button_height = 80;
    float center_x = internal_width / 2.0f - button_width / 2;
    float start_y = internal_height / 3.0f;
    float gap = 110;

    static button btn_continue({ center_x, start_y + 0 * gap, button_width, button_height });
    static button btn_save({ center_x, start_y + 1 * gap, button_width, button_height });
    static button btn_replay({ center_x, start_y + 2 * gap, button_width, button_height });
    static button btn_quit({ center_x, start_y + 3 * gap, button_width, button_height });

    handle_button_hover_draw(btn_continue, input.get_v_m(), { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_save, input.get_v_m(), { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_replay, input.get_v_m(), { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_quit, input.get_v_m(), { 255, 255, 255, 40 });

    DrawTextEx(pause_font, "Continue",
        { center_x + button_width / 2 - MeasureTextEx(pause_font, "Continue", font_size, 2).x / 2,
          start_y + 0 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(pause_font, "Save",
        { center_x + button_width / 2 - MeasureTextEx(pause_font, "Save", font_size, 2).x / 2,
          start_y + 1 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(pause_font, "Replay",
        { center_x + button_width / 2 - MeasureTextEx(pause_font, "Replay", font_size, 2).x / 2,
          start_y + 2 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(pause_font, "Quit",
        { center_x + button_width / 2 - MeasureTextEx(pause_font, "Quit", font_size, 2).x / 2,
          start_y + 3 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btn_continue.is_being_hovered_on(input.get_v_m()))
        {
            is_paused = false;
            return;
        }
        else if (btn_save.is_being_hovered_on(input.get_v_m()))
        {
            // std::cout << "SAVED GAME"; // Removed temporary debug output
        }
        else if (btn_replay.is_being_hovered_on(input.get_v_m()))
        {
            enter_replay_mode();
            is_paused = false;
            return;
        }
        else if (btn_quit.is_being_hovered_on(input.get_v_m()))
        {
            is_paused = false;
            quit = true;
            return;
        }
    }
}

void game_player::handle_button_hover_draw(button& btn, Vector2 mouse_pos, Color hover_color)
{
    if (btn.is_being_hovered_on(mouse_pos))
        btn.draw(hover_color);
}

void game_player::display_game_over()
{
    static Font game_over_font = LoadFont("assets/Montserrat-ExtraBold.ttf");
    static int font_size = 60;
    char message[100];

    if (b.is_stalemate())
    {
        
        snprintf(message, sizeof(message), "Stalemate! It's a draw!");
    }
    else
    {
        const char* winner_text = (t == white ? "Black" : "White");
        snprintf(message, sizeof(message), "Checkmate! %s wins!", winner_text);
    }

    DrawRectangle(0, internal_height / 2 - 50, internal_width, 100, Color{ 0, 0, 0, 200 });
    DrawTextEx(game_over_font, message,
        { internal_width / 2.0f - MeasureTextEx(game_over_font, message, font_size, 2).x / 2.0f,
          (float)internal_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        quit = true;
    }
}

void game_player::handle_pawn_promotion()
{
    if (!promotion_pending) return;

    int promotion_type = draw_promotion_ui();

    if (promotion_type != -1)
    {
        chess_move move = create_move_record(selected_pos, promotion_position);
        move.was_promotion = true;
        move.promotion_piece_type = promotion_type;

        // move_piece already handles piece deletion/placement
        b.move_piece(selected_pos, promotion_position); 
        b.promote_pawn(promotion_position, promotion_type);

        history.add_move(move);

        t = (t == white ? black : white);
        b.change_turn();
        cam.flip_camera();

        piece_selected = false;
        valid_moves.clear();
        promotion_pending = false;
    }
}

int game_player::draw_promotion_ui()
{
    static const int box_size = 100;
    static const int total_width = box_size * 4 + 30;
    static const int start_x = (internal_width - total_width) / 2;
    static const int y = internal_height / 2 - box_size / 2;

    static button queen_btn = button({ (float)start_x, (float)y, (float)box_size, (float)box_size });
    static button rook_btn = button({ (float)(start_x + box_size + 10), (float)y, (float)box_size, (float)box_size });
    static button bishop_btn = button({ (float)(start_x + 2 * (box_size + 10)), (float)y, (float)box_size, (float)box_size });
    static button knight_btn = button({ (float)(start_x + 3 * (box_size + 10)), (float)y, (float)box_size, (float)box_size });

    DrawRectangle(0, 0, internal_width, internal_height, Color{ 0, 0, 0, 180 });

    static Font promotion_font = LoadFont("assets/Montserrat-ExtraBold.ttf");
    int font_size = 40;
    const char* text = "Select piece for promotion:";
    DrawTextEx(promotion_font, text,
        { (float)(internal_width / 2.0f) - MeasureTextEx(promotion_font, text, font_size, 2).x / 2.0f,
        (float)internal_height / 2 - 150 },
        font_size, 2, WHITE);

    Color box_color = ColorAlpha(GRAY, 0.7f);
    Color hover_color = ColorAlpha(LIGHTGRAY, 0.9f);
    Color text_color = WHITE;

    handle_button_hover_draw(queen_btn, input.get_v_m(), hover_color);
    if (!queen_btn.is_being_hovered_on(input.get_v_m()))
        queen_btn.draw(box_color);

    handle_button_hover_draw(rook_btn, input.get_v_m(), hover_color);
    if (!rook_btn.is_being_hovered_on(input.get_v_m()))
        rook_btn.draw(box_color);

    handle_button_hover_draw(bishop_btn, input.get_v_m(), hover_color);
    if (!bishop_btn.is_being_hovered_on(input.get_v_m()))
        bishop_btn.draw(box_color);

    handle_button_hover_draw(knight_btn, input.get_v_m(), hover_color);
    if (!knight_btn.is_being_hovered_on(input.get_v_m()))
        knight_btn.draw(box_color);

    DrawTextEx(promotion_font, "Q", { (float)start_x + box_size / 2 - 15,(float)y + box_size / 2 - 20 }, 40, 2, text_color);
    DrawTextEx(promotion_font, "R", { (float)start_x + box_size + 10 + box_size / 2 - 15,(float)y + box_size / 2 - 20 }, 40, 2, text_color);
    DrawTextEx(promotion_font, "B", { (float)start_x + 2 * (box_size + 10) + box_size / 2 - 15,(float)y + box_size / 2 - 20 }, 40, 2, text_color);
    DrawTextEx(promotion_font, "N", { (float)start_x + 3 * (box_size + 10) + box_size / 2 - 15, (float)y + box_size / 2 - 20 }, 40, 2, text_color);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (queen_btn.is_being_hovered_on(input.get_v_m())) return 1;
        else if (rook_btn.is_being_hovered_on(input.get_v_m())) return 2;
        else if (bishop_btn.is_being_hovered_on(input.get_v_m())) return 3;
        else if (knight_btn.is_being_hovered_on(input.get_v_m())) return 4;
    }

    return -1;
}

chess_move game_player::create_move_record(position from, position to)
{
    chess_move move = {};
    move.from = from;
    move.to = to;
    move.captured_piece_type = -1;
    move.captured_piece_color = -1;
    move.promotion_piece_type = -1;
    move.was_castling = false;
    move.was_promotion = false;
    move.was_en_passant = false;

    piece* moving_piece = b.get_piece(from);
    if (moving_piece)
    {
        move.piece_type = moving_piece->get_type();
        move.piece_color = moving_piece->get_clr();
    }

    piece* captured_piece = b.get_piece(to);
    if (captured_piece)
    {
        move.captured_piece_type = captured_piece->get_type();
        move.captured_piece_color = captured_piece->get_clr();
    }
    else if (moving_piece && moving_piece->get_type() == 0 && to == b.get_en_passant_target()) {
        // En passant capture
        move.was_en_passant = true;
        // The actual captured pawn is not at 'to', but one square behind it
        if (moving_piece->get_clr() == white) {
            move.captured_piece_type = b.get_piece(position(to.x + 1, to.y))->get_type();
            move.captured_piece_color = b.get_piece(position(to.x + 1, to.y))->get_clr();
        } else {
            move.captured_piece_type = b.get_piece(position(to.x - 1, to.y))->get_type();
            move.captured_piece_color = b.get_piece(position(to.x - 1, to.y))->get_clr();
        }
    }


    if (moving_piece && moving_piece->get_type() == 5) // King
    {
        move.was_castling = (std::abs(from.y - to.y) == 2);
    }

    move.king_moved_before = b.has_king_moved(move.piece_color);
    move.rook_kingside_moved_before = b.has_rook_moved(move.piece_color, true);
    move.rook_queenside_moved_before = b.has_rook_moved(move.piece_color, false);
    move.en_passant_target = b.get_en_passant_target(); // Current en passant target before this move

    return move;
}

void game_player::undo_last_move()
{
    if (!history.can_undo() || replay_mode)
        return;

    const chess_move* move = history.get_move(history.get_current_move_index());
    if (!move)
        return;

    reverse_move(*move);

    history.undo_move();

    t = (t == white ? black : white);
    b.change_turn();
    cam.flip_camera();

    piece_selected = false;
    valid_moves.clear();
}

void game_player::redo_move()
{
    if (!history.can_redo() || replay_mode)
        return;

    if (history.redo_move())
    {
        const chess_move* move = history.get_move(history.get_current_move_index());
        if (move)
        {
            replay_single_move(*move);

            t = (t == white ? black : white);
            b.change_turn();
            cam.flip_camera();
        }
    }
}

void game_player::enter_replay_mode()
{
    if (history.get_total_moves() == 0)
        return;

    replay_mode = true;
    saved_turn = t;

    // Reset board and rebuild from history
    b.reset(true);
    history.reset_to_start();
    t = white; // Replay always starts with white
    b.change_turn(); // Flip back to white to start
    cam.reset(); // Reset camera
    cam.flip_camera(); // Flip camera to white's side for start of replay

    piece_selected = false;
    valid_moves.clear();
}

void game_player::exit_replay_mode()
{
    if (!replay_mode)
        return;

    replay_mode = false;
    restore_to_current_position();
    t = saved_turn; // Restore original turn
    b.change_turn(); // Ensure board's turn matches restored 't'
    cam.reset(); // Reset camera
    // if (t == black) cam.flip_camera(); // If it was black's turn, flip to black's side

    piece_selected = false;
    valid_moves.clear();
}

void game_player::replay_step_forward()
{
    if (!replay_mode) return;

    if (history.step_forward())
    {
        const chess_move* move = history.get_move(history.get_current_move_index());
        if (move)
        {
            replay_single_move(*move);
            t = (t == white ? black : white); // Update player turn
            b.change_turn(); // Update board turn
            cam.flip_camera(); // Flip camera view
        }
    }
}

void game_player::replay_step_backward()
{
    if (!replay_mode)
        return;

    if (history.step_backward()) // This means we moved to the *previous* move in history
    {
        rebuild_to_current_position(); // Rebuilds the board up to the new current_move_index
        t = (t == white ? black : white); // Update player turn
        b.change_turn(); // Update board turn
        cam.flip_camera(); // Flip camera view
    } else {
        // If we are at the very beginning, reset to initial board state
        b.reset(true);
        t = white;
        b.change_turn();
        cam.reset();
    }
}

    void game_player::reverse_move(const chess_move& move)
{
    // Restore en passant target before the move
    b.set_en_passant_target(move.en_passant_target);

    // Restore captured piece if any (and it wasn't en passant)
    if (move.captured_piece_type != -1 && !move.was_en_passant)
    {
        b.place_piece(move.to, move.captured_piece_type, move.captured_piece_color);
    }
    // Handle en passant capture reversal
    else if (move.was_en_passant) {
        // Place back the captured pawn
        if (move.piece_color == white) { // White pawn captured black pawn
            b.place_piece(position(move.to.x + 1, move.to.y), move.captured_piece_type, move.captured_piece_color);
        } else { // Black pawn captured white pawn
            b.place_piece(position(move.to.x - 1, move.to.y), move.captured_piece_type, move.captured_piece_color);
        }
    }

    // Move piece back from 'to' to 'from'
    if (move.was_promotion) // If it was a promotion, the piece at 'to' is promoted, need to revert to pawn
    {
        b.place_piece(move.from, 0, move.piece_color); // Place original pawn back
    }
    else {
        b.place_piece(move.from, move.piece_type, move.piece_color); // Place original piece back
    }

    // Handle castling reversal
    if (move.was_castling) {
        bool kingside = move.to.y > move.from.y;
        int rank = move.from.x;
        if (kingside) {
            b.place_piece(position(rank, 7), 1, move.piece_color); // Place rook back
            b.set_piece_at(position(rank, 5), nullptr); // Clear rook's moved-to square
        }
        else {
            b.place_piece(position(rank, 0), 1, move.piece_color); // Place rook back
            b.set_piece_at(position(rank, 3), nullptr); // Clear rook's moved-to square
        }
    }
    
    // Restore piece_moved flags
    b.set_king_moved(move.piece_color, move.king_moved_before);
    b.set_rook_moved(move.piece_color, true, move.rook_kingside_moved_before);
    b.set_rook_moved(move.piece_color, false, move.rook_queenside_moved_before);
}

void game_player::replay_single_move(const chess_move& move)
{
    // Clear captured piece at destination if any (for non-en-passant)
    b.remove_piece(move.to);
    
    // Handle en passant capture
    if (move.was_en_passant) {
        if (move.piece_color == white) { // White pawn captured black pawn
            b.remove_piece(position(move.to.x + 1, move.to.y));
        } else { // Black pawn captured white pawn
            b.remove_piece(position(move.to.x - 1, move.to.y));
        }
    }

    // Move piece (this also updates its internal position)
    piece* p = b.get_piece(move.from);
    if (p) { // Ensure piece exists before moving
        p->move(move.to);
        b.set_piece_at(move.to, p);
        b.set_piece_at(move.from, nullptr);
    }

    // Handle promotion
    if (move.was_promotion)
    {
        b.promote_pawn(move.to, move.promotion_piece_type);
    }

    // Handle castling
    if (move.was_castling)
    {
        bool kingside = move.to.y > move.from.y;
        int rank = move.from.x;
        if (kingside)
        {
            piece* rook_piece = b.get_piece(position(rank, 7));
            if (rook_piece) {
                rook_piece->move(position(rank, 5));
                b.set_piece_at(position(rank, 5), rook_piece);
                b.set_piece_at(position(rank, 7), nullptr);
            }
        }
        else
        {
            piece* rook_piece = b.get_piece(position(rank, 0));
            if (rook_piece) {
                rook_piece->move(position(rank, 3));
                b.set_piece_at(position(rank, 3), rook_piece);
                b.set_piece_at(position(rank, 0), nullptr);
            }
        }
    }    // Update en passant target for the current board state after this move
    if (move.piece_type == 0 && std::abs(move.from.x - move.to.x) == 2) { // Pawn moved two squares
        b.set_en_passant_target(position((move.from.x + move.to.x) / 2, move.from.y));
    } else {
        b.set_en_passant_target(position(-1, -1));
    }
}


void game_player::rebuild_to_current_position()
{
    b.reset(true); // Reset board to initial state
    t = white; // Start with white's turn for rebuild

    // Replay all moves up to current_move_index
    for (int i = 0; i <= history.get_current_move_index(); i++)
    {
        const chess_move* move = history.get_move(i);
        if (move)
        {
            replay_single_move(*move);
            t = (t == white ? black : white); // Update player turn
            b.change_turn(); // Update board turn
        }
    }
}

void game_player::restore_to_current_position()
{
    // Rebuild the board state from the beginning up to the last move in history
    rebuild_to_current_position();
    // Set the current_move_index to the end of the history
    history.jump_to_move(history.get_total_moves() - 1);
    // Ensure the turn matches the state after the last move
    if (history.get_total_moves() % 2 == 0) { // Even number of moves means white's turn
        t = white;
    } else { // Odd number of moves means black's turn
        t = black;
    }
    b.change_turn(); // Sync board's current_turn
}

void game_player::draw_replay_controls()
{
    if (!replay_mode) return;

    DrawRectangle(10, internal_height - 60, internal_width - 20, 50, Color{ 0, 0, 0, 150 });

    static Font controls_font = LoadFont("assets/Montserrat-ExtraBold.ttf");
    const char* text = "REPLAY MODE - Arrow Keys: Step | ESC: Exit | SPACE: Play/Pause";
    DrawTextEx(controls_font, text, { 20, internal_height - 45 }, 20, 1, WHITE);

    char move_info[50];
    snprintf(move_info, sizeof(move_info), "Move %d / %d", history.get_current_move_index() + 1, history.get_total_moves());
    DrawTextEx(controls_font, move_info, { internal_width - 150, internal_height - 45 }, 20, 1, WHITE);
}

void game_player::reset(bool has_loaded)
{
    cam.reset();
    b.reset(has_loaded);
    t = white;
    is_paused = false;
    piece_selected = false;
    promotion_pending = false;
    replay_mode = false;
    quit = false;
    valid_moves.clear();
    history.clear_history(); // Clear move history on reset
}

bool game_player::play(audio_manager& audio)
{
    quit = false;

    if (!replay_mode)
    {
        cam.handle_camera_movement();
    }

    if (b.is_checkmate() || b.is_stalemate())
    {
        audio.play_checkmate(); // Or play draw sound for stalemate
        if (!replay_mode)
        {
            display_game_over();
            return quit;
        }
    }

    if (!is_paused && !replay_mode)
    {
        position clicked_pos = get_board_position_from_mouse();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (!piece_selected)
            {
                handle_piece_selection(clicked_pos);
            }
            else
            {
                handle_piece_move(clicked_pos, audio);
            }
        }

        if (promotion_pending)
        {
            handle_pawn_promotion();
            return quit; // Stay in current loop until promotion is handled
        }

        if (IsKeyPressed(KEY_U))
            undo_last_move();
        if (IsKeyPressed(KEY_R))
            redo_move();
        if (IsKeyPressed(KEY_T))
        {
            b.cycle_palette();
        }
    }

    if (replay_mode)
    {
        if (IsKeyPressed(KEY_LEFT))
            replay_step_backward();
        if (IsKeyPressed(KEY_RIGHT))
            replay_step_forward();
        if (IsKeyPressed(KEY_ESCAPE))
            exit_replay_mode();
    }

    BeginMode3D(cam.get_camera());

    b.print_board();
    if (!replay_mode)
        draw_board_highlights();
    b.draw_pieces();

    EndMode3D();

    if (replay_mode)
        draw_replay_controls();

    if (IsKeyPressed(KEY_P) && !replay_mode)
        is_paused = true;
    if (is_paused)
    {
        game_paused();
        return quit;
    }

    return quit;
}