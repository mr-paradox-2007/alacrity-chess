#include"BSAI24056_chess.h"

void chess::setup_window()
{
    SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(internal_width, internal_height, "Ammar's Awesome 3D Chess");

    SetWindowSize(internal_width, internal_height);
    //SetWindowSize(screen_width, screen_height);

    display_screen = LoadRenderTexture(internal_width, internal_height);
    SetTextureFilter(display_screen.texture, TEXTURE_FILTER_BILINEAR);
    SetTargetFPS(60);
}
void chess::main_loop()
{
    while (!exit_game)
    {
        audio.update();
        input.update_mouse();

		BeginTextureMode(display_screen);
		ClearBackground(BLACK);

        if (is_main_menu)
        {
            main_menu();
        }
        else if (is_settings)
        {
            settings();
        }
        else if (is_playing)
        {
            if (game.play(audio))
            {
                is_playing = false;
                is_main_menu = true;
            }
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(
            display_screen.texture,
            { 0, 0,
              (float)display_screen.texture.width,
              (float)-display_screen.texture.height },
            { (GetScreenWidth() - internal_width * input.get_scale()) * 0.5f,
              (GetScreenHeight() - internal_height * input.get_scale()) * 0.5f,
              internal_width * input.get_scale(),
              internal_height * input.get_scale() },
            { 0, 0 },
            0.0f,
            WHITE
        );
        EndDrawing();
    }
}

void chess::main_menu()
{
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

    DrawTexturePro(menu_background,
        { 0, 0, (float)menu_background.width, (float)menu_background.height },
        { 0, 0, (float)internal_width, (float)internal_height },
        { 0, 0 },
        0,
        WHITE
    );

    handle_button_hover_draw(btn_start, input.get_v_m());
    handle_button_hover_draw(btn_new_game, input.get_v_m());
    handle_button_hover_draw(btn_load_game, input.get_v_m());
    handle_button_hover_draw(btn_settings, input.get_v_m());
    handle_button_hover_draw(btn_exit, input.get_v_m());

    DrawTextEx(menu_font, "Start", { left_x,  top_y }, font_size, 2, WHITE);
    DrawTextEx(menu_font, "New Game", { right_x, top_y }, font_size, 2, WHITE);
    DrawTextEx(menu_font, "Load Game", { left_x,  top_y + gap }, font_size, 2, WHITE);
    DrawTextEx(menu_font, "Settings", { right_x, top_y + gap }, font_size, 2, WHITE);
    DrawTextEx(menu_font, "Exit", { internal_width / 2.0f - MeasureText("Exit",font_size) / 2, top_y + 2 * gap + 240 }, font_size, 2, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btn_exit.is_being_hovered_on(input.get_v_m()))
        {
            audio.play_not_good();
			exit_game = true;
            is_main_menu = false;
            return;
        }
        if (btn_settings.is_being_hovered_on(input.get_v_m()))
        {
            audio.play_sub_menu();
            is_settings = true;
            is_main_menu = false;
            return;
        }
        if (btn_start.is_being_hovered_on(input.get_v_m()))
        {
            if (game_loaded)
            {
                audio.play_menu_click();
                is_main_menu = false;
                is_playing = true;
            }
            else
            {
                audio.play_not_good();
            }
            return;
        }
        if (btn_new_game.is_being_hovered_on(input.get_v_m()))
        {
            audio.play_menu_click();
            game.reset(game_loaded);
            game_loaded = true;
            return;
        }
        if (btn_load_game.is_being_hovered_on(input.get_v_m()))
        {
            audio.play_menu_click();
            return;
        }
    }
}
void chess::settings()
{
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

    DrawTexturePro(settings_background,
        { 0, 0, (float)settings_background.width, (float)settings_background.height },
        { 0, 0, (float)internal_width, (float)internal_height },
        { 0, 0 },
        0,
        WHITE);

    handle_button_hover_draw(btn_music, input.get_v_m(), {255, 255, 255, 40});
    handle_button_hover_draw(btn_save_data, input.get_v_m(), { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_erase_data, input.get_v_m(), { 255, 255, 255, 40 });
    handle_button_hover_draw(btn_back, input.get_v_m(), { 255, 255, 255, 40 });

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

    DrawTextEx(settings_font, "Back",
        { center_x + button_width / 2 - MeasureTextEx(settings_font, "Back", font_size, 2).x / 2,
          start_y + 4 * gap + button_height / 2 - font_size / 2 },
        font_size, 2, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btn_music.is_being_hovered_on(input.get_v_m()))
        {
            music_on = !music_on;
            if (music_on)
            {
                audio.play_bgm();
            }
            else
            {
                audio.stop_bgm();
            }
        }
        else if (btn_save_data.is_being_hovered_on(input.get_v_m()))
        {
        }
        else if (btn_erase_data.is_being_hovered_on(input.get_v_m()))
        {
        }
        else if (btn_back.is_being_hovered_on(input.get_v_m()))
        {
            is_settings = false;
            is_main_menu = true;
        }
    }
}

void chess::handle_button_hover_draw(button& btn, Vector2 mouse_pos)
{
    if (btn.is_being_hovered_on(mouse_pos))
        btn.draw({ 0, 0, 0, 40 });
}
void chess::handle_button_hover_draw(button& btn, Vector2 mouse_pos, Color hover_color)
{
    if (btn.is_being_hovered_on(mouse_pos))
        btn.draw(hover_color);
}

chess::chess()
{
    is_main_menu = true;
    is_settings = false;
    is_playing = false;
    is_paused = false;
    exit_game = false;
    game_loaded = false;
    music_on = true;
}
chess::~chess()
{
}
void chess::play()
{
    setup_window();
    audio.play_bgm();
    main_loop();
    if(music_on)
    audio.stop_bgm();
    CloseWindow();
}
