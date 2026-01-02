#include "BSAI24056_play.h"

play::play() :
    sub_ok_he_boss(true),
    fresh_game(false),
    show_fps(false),
    exit_game(false),
    is_paused(false),
    is_main_menu(true),
    is_settings(false),
    music_on(true),
    autosave_on(true),
    saved_board(false),
    is_main_game(false),
    camera({ 0 }),
    camera_angle_y(0.0f),
    camera_angle_x(0.0f),
    camera_target({ 0.0f, 0.0f, 0.0f }),
    promotion_pending(false),
    promotion_position({ 0, 0 }),
    flipped(false),
    current_turn(white),
    selected_pos({ 0, 0 }),
    pos({ 0, 0 }),
    piece_selected(false)
{

    load_settings();

    if (fresh_game)
    {
        is_main_menu = false;
        return;
    }
}

play::~play()
{
    UnloadRenderTexture(display_screen);
}

void play::load_settings()
{
    std::ifstream file("assets/BSAI24056_data.txt");
    if (!file.is_open())
    {

        fresh_game = true;
        music_on = true;
        autosave_on = true;
        players[0].set_name("White");
        players[1].set_name("Black");
        players[0].elo = 1200;
        players[1].elo = 1200;
        save_settings();
        return;
    }

    int fg, mo, ao;
    file >> fg >> mo >> ao;
    file.get();

    fresh_game = (fg == 1);
    music_on = (mo == 1);
    autosave_on = (ao == 1);

    std::string line;

    std::getline(file, line);
    players[0].set_name(line);
    file >> players[0].elo;
    file.get();

    std::getline(file, line);
    players[1].set_name(line);
    file >> players[1].elo;
    file.get();

    file.close();
    toggle_music(music_on);
}

void play::save_settings()
{
    std::ofstream file("assets/BSAI24056_data.txt");
    if (!file.is_open()) return;

    file << (fresh_game ? 1 : 0) << ' '
        << (music_on ? 1 : 0) << ' '
        << (autosave_on ? 1 : 0) << '\n';

    file << players[0].get_name() << '\n'
        << players[0].elo << '\n';

    file << players[1].get_name() << '\n'
        << players[1].elo << '\n';
}

void play::manage_flags()
{
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
    if (IsKeyPressed(KEY_F2))  show_fps = !show_fps;
    if (IsKeyPressed(KEY_T))   b.cycle_palette();
}

void play::manage_mouse()
{
    scale = fminf((float)GetScreenWidth() / internal_width,
        (float)GetScreenHeight() / internal_height);

    mouse = GetMousePosition();
    virtual_mouse = {
        (mouse.x - (GetScreenWidth() - internal_width * scale) * 0.5f) / scale,
        (mouse.y - (GetScreenHeight() - internal_height * scale) * 0.5f) / scale
    };
    virtual_mouse = Vector2Clamp(virtual_mouse,
        { 0, 0 },
        { (float)internal_width, (float)internal_height });
}

void play::fresh_start_screen()
{
    static int input_phase = 0;
    static char buffer[64] = { 0 };
    static int buffer_len = 0;

    ClearBackground(BLACK);

    if (input_phase == 0) {
        DrawText("Enter WHITE player's name:", 100, 150, 30, RAYWHITE);
    }
    else if (input_phase == 1) {
        DrawText("Enter BLACK player's name:", 100, 150, 30, RAYWHITE);
    }
    else if (input_phase == 2) {
        DrawText("Enter WHITE player's starting ELO:", 100, 150, 30, RAYWHITE);
    }
    else if (input_phase == 3) {
        DrawText("Enter BLACK player's starting ELO:", 100, 150, 30, RAYWHITE);
    }

    DrawText(buffer, 100, 200, 30, YELLOW);

    int key = GetCharPressed();
    while (key > 0) {

        if (buffer_len < 63 &&
            ((input_phase < 2 && key >= 32 && key <= 125) ||
                (input_phase >= 2 && key >= '0' && key <= '9')))
        {
            buffer[buffer_len++] = (char)key;
            buffer[buffer_len] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && buffer_len > 0) {
        buffer[--buffer_len] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER) && buffer_len > 0) {
        if (input_phase == 0) {
            players[0].set_name(buffer);
        }
        else if (input_phase == 1) {
            players[1].set_name(buffer);
        }
        else if (input_phase == 2) {
            players[0].set_elo(std::stoi(buffer));
        }
        else if (input_phase == 3) {
            players[1].set_elo(std::stoi(buffer));
        }

        buffer_len = 0;
        buffer[0] = '\0';
        input_phase++;
    }

    if (input_phase > 3) {
        fresh_game = false;
        is_main_game = true;
        input_phase = 0;
        save_settings();
    }
}

void play::main_menu()
{
    if (is_settings)
    {
        settings_menu();
        return;
    }

    static Texture2D menu_background = LoadTexture("assets/main_menu_bg.png");
    static Font menu_font = LoadFont("assets/Montserrat-ExtraBold.ttf");

    static int font_size = 50;
    float left_x = 20;
    float right_x = 1550;
    float top_y = internal_height / 3.0f;
    float gap = 160;

    static button btn_start({ left_x, top_y, 350, 80 });
    static button btn_new_game({ right_x, top_y, 350, 80 });
    static button btn_load_game({ left_x, top_y + gap, 350, 80 });
    static button btn_settings({ right_x, top_y + gap, 350, 80 });
    static button btn_exit({ internal_width / 2.0f - 150, top_y + 2 * gap + 235, 300, 60 });

    ClearBackground(BLACK);

    DrawTexturePro(menu_background,
        { 0, 0, (float)menu_background.width, (float)menu_background.height },
        { 0, 0, (float)internal_width, (float)internal_height },
        { 0, 0 },
        0,
        WHITE);

    handle_button_hover_draw(btn_start, virtual_mouse);
    handle_button_hover_draw(btn_new_game, virtual_mouse);
    handle_button_hover_draw(btn_load_game, virtual_mouse);
    handle_button_hover_draw(btn_settings, virtual_mouse);
    handle_button_hover_draw(btn_exit, virtual_mouse);

    DrawTextEx(menu_font, "Start", { left_x,  top_y }, font_size, 2, WHITE);
    DrawTextEx(menu_font, "New Game", { right_x, top_y }, font_size, 2, WHITE);
    DrawTextEx(menu_font, "Load Game", { left_x,  top_y + gap }, font_size, 2, WHITE);
    DrawTextEx(menu_font, "Settings", { right_x, top_y + gap }, font_size, 2, WHITE);
    DrawTextEx(menu_font, "Exit", { internal_width / 2.0f - MeasureText("Exit",font_size) / 2, top_y + 2 * gap + 240 }, font_size, 2, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btn_exit.is_being_hovered_on(virtual_mouse))
        {
            CloseWindow();
            exit_game = true;
            return;
        }
        if (btn_settings.is_being_hovered_on(virtual_mouse))
        {
            is_settings = true;
            return;
        }
        if (btn_start.is_being_hovered_on(virtual_mouse))
        {
            is_main_game = true;
            is_main_menu = false;
            return;
        }
        if (btn_new_game.is_being_hovered_on(virtual_mouse))
        {
          
            b.reset(true);

            current_turn = white;
            flipped = false;
            camera_angle_x = 0.0f;
            camera_angle_y = 0.0f;
            camera_target = { 0.0f, 0.0f, 0.0f };

            valid_moves.clear();
            piece_selected = false;

            is_main_game = true;
            is_main_menu = false;
            return;
        }

        if (btn_load_game.is_being_hovered_on(virtual_mouse))
        {
            load_game();
            return;
        }
    }
}

void play::handle_button_hover_draw(button& btn, Vector2 mouse_pos)
{
    if (btn.is_being_hovered_on(mouse_pos))
        btn.draw({ 0, 0, 0, 40 });
}

void play::settings_menu()
{
    if (!is_settings)
        return;
    static Texture2D settings_background = LoadTexture("assets/settings_bg.png");
    static Font settings_font = LoadFont("assets/Montserrat-ExtraBold.ttf");
    static int font_size = 50;

    float button_width = 450;
    float button_height = 80;
    float center_x = internal_width / 2.0f - button_width / 2;
    float start_y = internal_height / 3.0f;
    float gap = 110;

    static button btn_music({ center_x, start_y + 0 * gap, button_width, button_height });
    static button btn_save_data({ center_x, start_y + 1 * gap, button_width, button_height });
    static button btn_erase_data({ center_x, start_y + 2 * gap, button_width, button_height });
    static button btn_autosave({ center_x, start_y + 3 * gap, button_width, button_height });
    static button btn_back({ center_x, start_y + 4 * gap, button_width, button_height });

    ClearBackground(BLACK);

    DrawTexturePro(settings_background,
        { 0, 0, (float)settings_background.width, (float)settings_background.height },
        { 0, 0, (float)internal_width, (float)internal_height },
        { 0, 0 },
        0,
        WHITE);

    handle_button_hover_draw(btn_music, virtual_mouse, { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_save_data, virtual_mouse, { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_erase_data, virtual_mouse, { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_autosave, virtual_mouse, { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_back, virtual_mouse, { 255, 255, 255, 40 });

    DrawTextEx(settings_font, music_on ? "Music: ON" : "Music: OFF",
        { center_x + button_width / 2 - MeasureTextEx(settings_font, "Music: OFF", font_size, 2).x / 2,
          start_y + 0 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(settings_font, "Save Data",
        { center_x + button_width / 2 - MeasureTextEx(settings_font, "Save Data", font_size, 2).x / 2,
          start_y + 1 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(settings_font, "Erase Data",
        { center_x + button_width / 2 - MeasureTextEx(settings_font, "Erase Data", font_size, 2).x / 2,
          start_y + 2 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(settings_font, autosave_on ? "Auto-Save: ON" : "Auto-Save: OFF",
        { center_x + button_width / 2 - MeasureTextEx(settings_font, "Auto-Save: OFF", font_size, 2).x / 2,
          start_y + 3 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(settings_font, "Back",
        { center_x + button_width / 2 - MeasureTextEx(settings_font, "Back", font_size, 2).x / 2,
          start_y + 4 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btn_music.is_being_hovered_on(virtual_mouse))
        {
            music_on = !music_on;
            toggle_music(music_on);
            save_settings();
        }
        else if (btn_autosave.is_being_hovered_on(virtual_mouse))
        {
            autosave_on = !autosave_on;
            save_settings();
        }
        else if (btn_save_data.is_being_hovered_on(virtual_mouse))
        {
            save_settings();
        }
        else if (btn_erase_data.is_being_hovered_on(virtual_mouse))
        {
            erase_data();
        }
        else if (btn_back.is_being_hovered_on(virtual_mouse))
        {
            is_settings = false;
        }
    }
    if (IsKeyPressed(KEY_P))
    {
        is_settings = false;
    }
}

void play::handle_button_hover_draw(button& btn, Vector2 mouse_pos, Color hover_color)
{
    if (btn.is_being_hovered_on(mouse_pos))
        btn.draw(hover_color);
}

void play::game_paused()
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
    static button btn_settings({ center_x, start_y + 2 * gap, button_width, button_height });
    static button btn_quit({ center_x, start_y + 3 * gap, button_width, button_height });

    handle_button_hover_draw(btn_continue, virtual_mouse, { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_save, virtual_mouse, { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_settings, virtual_mouse, { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_quit, virtual_mouse, { 255, 255, 255, 40 });

    DrawTextEx(pause_font, "Continue",
        { center_x + button_width / 2 - MeasureTextEx(pause_font, "Continue", font_size, 2).x / 2,
          start_y + 0 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(pause_font, "Save",
        { center_x + button_width / 2 - MeasureTextEx(pause_font, "Save", font_size, 2).x / 2,
          start_y + 1 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(pause_font, "Settings",
        { center_x + button_width / 2 - MeasureTextEx(pause_font, "Settings", font_size, 2).x / 2,
          start_y + 2 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    DrawTextEx(pause_font, "Quit",
        { center_x + button_width / 2 - MeasureTextEx(pause_font, "Quit", font_size, 2).x / 2,
          start_y + 3 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btn_continue.is_being_hovered_on(virtual_mouse))
        {
            is_paused = false;
            return;
        }
        else if (btn_save.is_being_hovered_on(virtual_mouse))
        {
            save_game();
        }
        else if (btn_settings.is_being_hovered_on(virtual_mouse))
        {
            is_settings = true;
        }
        else if (btn_quit.is_being_hovered_on(virtual_mouse))
        {
            is_paused = false;
            is_main_game = false;
            is_main_menu = true;
        }
    }
}

void play::handle_camera_movement()
{
    if (IsKeyDown(KEY_UP))   camera_angle_y += 0.01f;
    if (IsKeyDown(KEY_DOWN)) camera_angle_y -= 0.01f;
    camera_angle_y = Clamp(camera_angle_y, -PI * 0.05f, PI * 0.4f);
    if (IsKeyPressed(KEY_SPACE)) camera_angle_x += PI;

    const float r = 10.0f, c_height = 2.0f;
    Vector3 camPos = {
        r * cosf(camera_angle_y) * sinf(camera_angle_x),
        r * sinf(camera_angle_y) + c_height,
        r * cosf(camera_angle_y) * cosf(camera_angle_x)
    };
    camera.position = camPos;
    camera.target = camera_target;
    camera.up = { 0, 1, 0 };
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;
}

position play::get_board_position_from_mouse()
{
    Ray ray = GetMouseRay(mouse, camera);
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

void play::handle_piece_selection(position pos)
{
    if (b.is_valid_source(pos))
    {
        piece* p = b.get_piece(pos);
        if (p && p->get_clr() == current_turn)
        {
            piece_selected = true;
            selected_pos = pos;

            valid_moves.clear();
            calculate_valid_moves();
        }
    }
}

void play::calculate_valid_moves()
{
    piece* p = b.get_piece(selected_pos);
    if (!p) return;

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

            if (p->get_type() == 5) {

                if (dest_pos.x == selected_pos.x && dest_pos.y == selected_pos.y + 2
                    && b.can_castle_kingside(p->get_clr()))
                {
                    valid_moves.push_back(dest_pos);
                }

                if (dest_pos.x == selected_pos.x && dest_pos.y == selected_pos.y - 2
                    && b.can_castle_queenside(p->get_clr()))
                {
                    valid_moves.push_back(dest_pos);
                }
            }

        }
    }
}

void play::handle_piece_move(position pos)
{


    if (pos == selected_pos)
    {
        piece_selected = false;
        valid_moves.clear();
        return;
    }

    if (pos.x >= 0 && pos.x < 8 && pos.y >= 0 && pos.y < 8)
    {
        piece* p = b.get_piece(selected_pos);

        if (b.get_piece(pos) && b.get_piece(pos)->get_clr() == current_turn)
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

            b.move_piece(selected_pos, pos);

            current_turn = (current_turn == white ? black : white);
            b.change_turn();
            camera_angle_x += PI;

            piece_selected = false;
            valid_moves.clear();

            if (autosave_on)
            {
                save_game_autosave();
            }
        }
    }

    if (b.is_checkmate() || b.is_stalemate()) {
        display_game_over();
        return;
    }

}

void play::display_game_over()
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
        const char* winner_text = (current_turn == white ? "Black" : "White");
        sprintf_s(message, "Checkmate! %s wins!", winner_text);
    }

    DrawRectangle(0, internal_height / 2 - 50, internal_width, 100, Color{ 0, 0, 0, 200 });
    DrawTextEx(game_over_font, message,
        { internal_width / 2.0f - MeasureTextEx(game_over_font, message, font_size, 2).x / 2.0f,
          (float)internal_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        bool white_won = b.is_checkmate() && current_turn == black;
        bool draw = b.is_stalemate();

        const int winner_elo_bump = 10;
        const int draw_elo_bump = 5;

        if (white_won)
        {
            players[0].elo += winner_elo_bump;
        }
        else if (!white_won && !draw)
        {
            players[1].elo += winner_elo_bump;
        }
        else
        {
            players[0].elo += draw_elo_bump;
            players[1].elo += draw_elo_bump;
        }

        save_settings();

        is_main_game = false;
        is_main_menu = true;
    }
}

void play::draw_board_highlights()
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
                position rook_from = kingside ? position(rank, 7)
                    : position(rank, 0);
                position rook_to = kingside ? position(rank, 5)
                    : position(rank, 3);

                Vector3 rf = { off + rook_from.y * sq,
                               highlight_height,
                               off + rook_from.x * sq };
                Vector3 rt = { off + rook_to.y * sq,
                               highlight_height,
                               off + rook_to.x * sq };

                Color orangeAlpha = ColorAlpha(Color{ 255,165,0,255 }, 0.5f);
                DrawCube(rf, sq, 0.5f, sq, orangeAlpha);
                DrawCube(rt, sq, 0.5f, sq, orangeAlpha);
            }
        }
    }

    if (hover_pos.x >= 0 && hover_pos.x < 8 &&
        hover_pos.y >= 0 && hover_pos.y < 8)
    {
        piece* p = b.get_piece(hover_pos);
        if (p && p->get_clr() == current_turn)
        {
            Vector3 h = { off + hover_pos.y * sq,
                          highlight_height,
                          off + hover_pos.x * sq };
            DrawCube(h, sq, 0.5f, sq, ColorAlpha(SKYBLUE, 0.7f));
        }
    }
}

void play::draw_game_info()
{
    static Font info_font = LoadFont("assets/Montserrat-ExtraBold.ttf");
    const int font_size = 20;

    std::string white_info = players[0].get_name()
        + " (ELO: " + std::to_string(players[0].get_elo()) + ")";
    DrawTextEx(info_font,
        white_info.c_str(),
        { 20, 20 },
        font_size, 1, WHITE);

    std::string black_info = players[1].get_name()
        + " (ELO: " + std::to_string(players[1].get_elo()) + ")";
    Vector2 black_size = MeasureTextEx(info_font, black_info.c_str(), font_size, 1);
    DrawTextEx(info_font,
        black_info.c_str(),
        { internal_width - black_size.x - 20, 20 },
        font_size, 1, WHITE);

    const char* turn_text = (current_turn == white)
        ? "White's Turn"
        : "Black's Turn";
    Vector2 turn_size = MeasureTextEx(info_font, turn_text, font_size, 1);
    DrawTextEx(info_font,
        turn_text,
        { (internal_width - turn_size.x) / 2, 20 },
        font_size, 1, WHITE);

    if (b.is_check())
        DrawTextEx(info_font, "CHECK!", { (internal_width - MeasureTextEx(info_font, "CHECK!", font_size, 1).x) / 2, 50 }, font_size, 1, RED);
}

void play::main_game()
{
    handle_camera_movement();

    if (b.is_checkmate())
    {
        display_game_over();
        return;
    }

    if (b.is_stalemate())
    {
        display_game_over();
        return;
    }

    if (!is_paused)
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
                handle_piece_move(clicked_pos);
            }
        }

        if (promotion_pending)
        {
            handle_pawn_promotion();
            return;
        }
    }

    BeginMode3D(camera);

    b.print_board();
    draw_board_highlights();
    b.draw_pieces();

    EndMode3D();

    if (IsKeyPressed(KEY_P)) is_paused = !is_paused;
    if (is_paused)
    {
        if (is_settings)
        {
            settings_menu();
            return;
        }
        game_paused();
        return;
    }

    draw_game_info();
}

void play::play_game()
{
    if (!sub_ok_he_boss) return;

    SetConfigFlags(FLAG_FULLSCREEN_MODE| FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(internal_width, internal_height, "Ammar's Awesome 3D Chess");

    SetWindowSize(internal_width, internal_height);

    display_screen = LoadRenderTexture(internal_width, internal_height);
    SetTextureFilter(display_screen.texture, TEXTURE_FILTER_BILINEAR);
    SetTargetFPS(60);

    init_audio();

    while (!WindowShouldClose() && !exit_game)
    {
        manage_flags();
        manage_mouse();

        UpdateMusicStream(music);

        BeginTextureMode(display_screen);
        ClearBackground(BLACK);

        if (fresh_game)
            fresh_start_screen();
        else if (is_main_menu)
            main_menu();
        else if (is_main_game)
            main_game();

        if (show_fps)
            DrawFPS(10, 10);

        EndTextureMode();

        BeginDrawing();

        ClearBackground(BLACK);
        DrawTexturePro(
            display_screen.texture,
            { 0, 0,
              (float)display_screen.texture.width,
              (float)-display_screen.texture.height },
            { (GetScreenWidth() - internal_width * scale) * 0.5f,
              (GetScreenHeight() - internal_height * scale) * 0.5f,
              internal_width * scale,
              internal_height * scale },
            { 0, 0 },
            0.0f,
            WHITE
        );

        EndDrawing();
    }

    cleanup_audio();
    CloseWindow();
}

void play::save_game()
{
    std::ofstream file("chess_save.dat", std::ios::binary);
    if (!file.is_open())
    {
        return;
    }


    file.write(reinterpret_cast<char*>(&current_turn), sizeof(current_turn));
    file.write(reinterpret_cast<char*>(&flipped), sizeof(flipped));

    file.write(reinterpret_cast<char*>(&camera_angle_x), sizeof(camera_angle_x));
    file.write(reinterpret_cast<char*>(&camera_angle_y), sizeof(camera_angle_y));

    file.close();
    saved_board = true;
}

void play::save_game_autosave()
{
    if (!autosave_on) return;

    ofstream file("chess_autosave.dat", ios::binary);
    if (!file.is_open())
    {
        return;
    }

    file.write(reinterpret_cast<char*>(&current_turn), sizeof(current_turn));
    file.write(reinterpret_cast<char*>(&flipped), sizeof(flipped));
    file.write(reinterpret_cast<char*>(&camera_angle_x), sizeof(camera_angle_x));
    file.write(reinterpret_cast<char*>(&camera_angle_y), sizeof(camera_angle_y));

    file.close();
}

void play::handle_pawn_promotion()
{
    if (!promotion_pending) return;

    if (b.is_checkmate() || b.is_stalemate()) {
        display_game_over();
    }

    int promotion_type = draw_promotion_ui();

    if (promotion_type != -1) {
        b.move_piece(selected_pos, promotion_position);
        b.promote_pawn(promotion_position, promotion_type);

        current_turn = (current_turn == white ? black : white);
        b.change_turn();
        camera_angle_x += PI;

        piece_selected = false;
        valid_moves.clear();
        promotion_pending = false;

        if (autosave_on) {
            save_game_autosave();
        }

        if (b.is_checkmate()) {
            display_game_over();
        }
    }
}

void play::load_game()
{
    std::ifstream file("chess_save.dat", std::ios::binary);
    if (!file.is_open())
    {
        return;
    }


    if (file.read(reinterpret_cast<char*>(&current_turn), sizeof(current_turn)))
    {
        if (current_turn != white && current_turn != black)
        {
            current_turn = white;
        }
    }

    if (file.read(reinterpret_cast<char*>(&flipped), sizeof(flipped)))
    {

    }

    if (file.read(reinterpret_cast<char*>(&camera_angle_x), sizeof(camera_angle_x)))
    {
        if (std::isnan(camera_angle_x) || std::isinf(camera_angle_x))
        {
            camera_angle_x = 0.0f;
        }
    }

    if (file.read(reinterpret_cast<char*>(&camera_angle_y), sizeof(camera_angle_y)))
    {
        if (std::isnan(camera_angle_y) || std::isinf(camera_angle_y))
        {
            camera_angle_y = 0.0f;
        }
    }

    for (int i = 0; i < 2; i++)
    {

    }

    handle_camera_movement();

    file.close();
    is_main_menu = false;
    is_main_game = true;
    fresh_game = false;
    saved_board = true;
}

void play::erase_data()
{
    remove("chess_save.dat");
    remove("chess_autosave.dat");
    remove("assets/BSAI24056_data.txt");

    fresh_game = true;
    music_on = true;
    autosave_on = true;
    save_settings();
    saved_board = false;
}

void play::init_audio()
{
    InitAudioDevice();
    music = LoadMusicStream("assets/background_music.mp3");
    PlayMusicStream(music);
    SetMusicVolume(music, 0.5f);
}

void play::cleanup_audio()
{
    CloseAudioDevice();
}

void play::toggle_music(bool on)
{
    if (on)
    {
        ResumeMusicStream(music);
    }
    else
    {
        PauseMusicStream(music);
    }
}

int play::draw_promotion_ui()
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

    handle_button_hover_draw(queen_btn, virtual_mouse, hover_color);
    if (!queen_btn.is_being_hovered_on(virtual_mouse))
        queen_btn.draw(box_color);

    handle_button_hover_draw(rook_btn, virtual_mouse, hover_color);
    if (!rook_btn.is_being_hovered_on(virtual_mouse))
        rook_btn.draw(box_color);

    handle_button_hover_draw(bishop_btn, virtual_mouse, hover_color);
    if (!bishop_btn.is_being_hovered_on(virtual_mouse))
        bishop_btn.draw(box_color);

    handle_button_hover_draw(knight_btn, virtual_mouse, hover_color);
    if (!knight_btn.is_being_hovered_on(virtual_mouse))
        knight_btn.draw(box_color);

    DrawTextEx(promotion_font, "Q", { (float)start_x + box_size / 2 - 15,(float)y + box_size / 2 - 20 }, 40, 2, text_color);
    DrawTextEx(promotion_font, "R", { (float)start_x + box_size + 10 + box_size / 2 - 15,(float)y + box_size / 2 - 20 }, 40, 2, text_color);
    DrawTextEx(promotion_font, "B", { (float)start_x + 2 * (box_size + 10) + box_size / 2 - 15,(float)y + box_size / 2 - 20 }, 40, 2, text_color);
    DrawTextEx(promotion_font, "N", { (float)start_x + 3 * (box_size + 10) + box_size / 2 - 15, (float)y + box_size / 2 - 20 }, 40, 2, text_color);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (queen_btn.is_being_hovered_on(virtual_mouse)) return 1;
        else if (rook_btn.is_being_hovered_on(virtual_mouse)) return 2;
        else if (bishop_btn.is_being_hovered_on(virtual_mouse)) return 3;
        else if (knight_btn.is_being_hovered_on(virtual_mouse)) return 4;
    }

    return -1;
}