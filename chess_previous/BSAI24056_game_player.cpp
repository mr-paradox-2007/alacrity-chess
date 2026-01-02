#include "BSAI24056_game_player.h"

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

    if (fabs(denom) > 1e-6f)
    {
        Vector3 diff = Vector3Subtract(p0, ray.position);
        float t = Vector3DotProduct(diff, n) / denom;

        if (t >= 0)
        {
            Vector3 hit_point = Vector3Add(ray.position, Vector3Scale(ray.direction, t));

            int file = (int)roundf((hit_point.x - off) / sq);
            int rank = (int)roundf((hit_point.z - off) / sq);

            return position(rank, file);
        }
    }

    return position(-1, -1);
}

void game_player::handle_piece_selection(position pos)
{
    if (b.is_valid_source(pos))
    {
        piece* p = b.get_piece(pos);
        if (p && p->get_clr() == t)
        {
            piece_selected = true;
            selected_pos = pos;

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

    if (p->get_type() == 5)
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

void game_player::handle_piece_move(position pos, audio_manager& audio)
{
    if (replay_mode) return;

    if (pos == selected_pos) {
        piece_selected = false;
        valid_moves.clear();
        return;
    }

    if (b.is_valid_destination(pos))
    {
        piece* p = b.get_piece(selected_pos);
        if (b.get_piece(pos) && b.get_piece(pos)->get_clr() == t)
        {
            selected_pos = pos;
            valid_moves.clear();
            if (b.is_valid_destination(selected_pos))
                calculate_valid_moves();
            return;
        }

        bool is_castling = false;
        if (p && p->get_type() == 5)
        {
            if (selected_pos.x == pos.x && abs(selected_pos.y - pos.y) == 2)
            {
                is_castling = true;
            }
        }

        if ((p && (p->is_legal_move(pos) || is_castling) && !b.would_be_in_check_after_move(selected_pos, pos)) ||
            (is_castling && b.is_legal(selected_pos, pos)))
        {
            if (p->get_type() == 0)
            {
                if ((p->get_clr() == white && pos.x == 0) ||
                    (p->get_clr() == black && pos.x == 7))
                {
                    promotion_pending = true;
                    promotion_position = pos;
                    return;
                }
            }

            chess_move move = create_move_record(selected_pos, pos);

            b.move_piece(selected_pos, pos);

            

            history.add_move(move);

            if (t == white)
                audio.play_move_white();
            if (t == black)
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
        }
    }


    if (b.is_checkmate() || b.is_stalemate()) {
        return;
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
    if (piece_selected && selP && selP->get_type() == 5)
    {
        for (auto& mv : valid_moves)
        {
            if (mv.x == selected_pos.x && abs(mv.y - selected_pos.y) == 2)
            {
                bool kingside = mv.y > selected_pos.y;
                int rank = selected_pos.x;
                position rook_from = kingside ? position(rank, 7) : position(rank, 0);
                position rook_to = kingside ? position(rank, 5) : position(rank, 3);

                Vector3 rf = { off + rook_from.y * sq, highlight_height, off + rook_from.x * sq };
                Vector3 rt = { off + rook_to.y * sq, highlight_height, off + rook_to.x * sq };

                Color orangeAlpha = ColorAlpha(Color{ 255,165,0,255 }, 0.5f);
                DrawCube(rf, sq, 0.5f, sq, orangeAlpha);
                DrawCube(rt, sq, 0.5f, sq, orangeAlpha);
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
            cout << "SAVED GAME";
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
        
        sprintf_s(message, "Stalemate! It's a draw!");
    }
    else
    {
        const char* winner_text = (t == white ? "Black" : "White");
        sprintf_s(message, "Checkmate! %s wins!", winner_text);
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

    if (moving_piece && moving_piece->get_type() == 5)
    {
        move.was_castling = (abs(from.y - to.y) == 2);
    }

    move.king_moved_before = b.has_king_moved(move.piece_color);
    move.rook_kingside_moved_before = b.has_rook_moved(move.piece_color, true);
    move.rook_queenside_moved_before = b.has_rook_moved(move.piece_color, false);
    move.en_passant_target = b.get_en_passant_target();

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

    b.reset(true);
    history.reset_to_start();
    t = white;
    b.change_turn();
    cam.flip_camera();

    piece_selected = false;
    valid_moves.clear();
}

void game_player::exit_replay_mode()
{
    if (!replay_mode)
        return;

    replay_mode = false;
    restore_to_current_position();
    t = saved_turn;
    b.change_turn();

    cam.flip_camera();
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
            t = (t == white ? black : white);
        }
    }
}

void game_player::replay_step_backward()
{
    if (!replay_mode)
        return;

    if (history.step_backward())
    {
        rebuild_to_current_position();
    }
}

void game_player::reverse_move(const chess_move& move)
{
    if (move.was_promotion)
    {
        b.place_piece(move.to, 0, move.piece_color);
    }

    b.move_piece(move.to, move.from);

    if (move.captured_piece_type != -1)
    {
        b.place_piece(move.to, move.captured_piece_type, move.captured_piece_color);
    }

    if (move.was_castling) {
        bool kingside = move.to.y > move.from.y;
        int rank = move.from.x;
        if (kingside) {
            b.move_piece(position(rank, 5), position(rank, 7));
        }
        else {
            b.move_piece(position(rank, 3), position(rank, 0));
        }
    }

    b.set_king_moved(move.piece_color, move.king_moved_before);
    b.set_rook_moved(move.piece_color, true, move.rook_kingside_moved_before);
    b.set_rook_moved(move.piece_color, false, move.rook_queenside_moved_before);
    b.set_en_passant_target(move.en_passant_target);
}

void game_player::replay_single_move(const chess_move& move)
{
    b.move_piece(move.from, move.to);

    if (move.was_promotion)
    {
        b.promote_pawn(move.to, move.promotion_piece_type);
    }

    if (move.was_castling)
    {
        bool kingside = move.to.y > move.from.y;
        int rank = move.from.x;
        if (kingside)
        {
            b.move_piece(position(rank, 7), position(rank, 5));
        }
        else
        {
            b.move_piece(position(rank, 0), position(rank, 3));
        }
    }
}

void game_player::rebuild_to_current_position()
{
    b.reset(true);
    t = white;

    for (int i = 0; i <= history.get_current_move_index(); i++)
    {
        const chess_move* move = history.get_move(i);
        if (move)
        {
            replay_single_move(*move);
            t = (t == white ? black : white);
        }
    }
}

void game_player::restore_to_current_position()
{
    rebuild_to_current_position();
    history.jump_to_move(history.get_total_moves() - 1);
}

void game_player::draw_replay_controls()
{
    if (!replay_mode) return;

    DrawRectangle(10, internal_height - 60, internal_width - 20, 50, Color{ 0, 0, 0, 150 });

    static Font controls_font = LoadFont("assets/Montserrat-ExtraBold.ttf");
    const char* text = "REPLAY MODE - Arrow Keys: Step | ESC: Exit | SPACE: Play/Pause";
    DrawTextEx(controls_font, text, { 20, internal_height - 45 }, 20, 1, WHITE);

    char move_info[50];
    sprintf_s(move_info, "Move %d / %d", history.get_current_move_index() + 1, history.get_total_moves());
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
        audio.play_checkmate();
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
            return quit;
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