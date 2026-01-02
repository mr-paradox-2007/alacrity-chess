#ifndef BSAI24056_PLAY_H
#define BSAI24056_PLAY_H

#include "BSAI24056_utility.h"
#include "BSAI24056_player.h"
#include "BSAI24056_button.h"
#include "BSAI24056_board.h"

class play
{
private:
    bool sub_ok_he_boss;
    bool fresh_game;
    bool show_fps;
    bool exit_game;
    bool is_paused;
    bool is_main_menu;
    bool is_settings;
    bool music_on;
    bool autosave_on;
    bool saved_board;
    bool is_main_game;

    bool promotion_pending;
    position promotion_position;
    void handle_pawn_promotion();

    player players[2];

    Camera camera;
    float camera_angle_y;
    float camera_angle_x;
    Vector3 camera_target;
    bool flipped;

    turn current_turn;
    position selected_pos;
    position pos;
    bool piece_selected;
    std::vector<position> valid_moves;

    Vector2 virtual_mouse;
    Vector2 mouse;
    float scale;
    board b;
    RenderTexture2D display_screen;
    Music music;

    const int internal_width = 1920;
    const int internal_height = 1080;


    void load_settings();
    void save_settings();

    void manage_flags();
    void manage_mouse();
    void fresh_start_screen();
    void main_menu();
    void handle_button_hover_draw(button& btn, Vector2 mouse_pos);
    void handle_button_hover_draw(button& btn, Vector2 mouse_pos, Color hover_color);
    void settings_menu();
    void game_paused();
    void handle_camera_movement();
    position get_board_position_from_mouse();
    void handle_piece_selection(position pos);
    void calculate_valid_moves();
    void handle_piece_move(position pos);
    void display_game_over();
    void draw_board_highlights();
    void draw_game_info();
    void main_game();
    void save_game();
    void save_game_autosave();
    void load_game();
    void erase_data();
    void init_audio();
    void cleanup_audio();
    void toggle_music(bool on);
    int draw_promotion_ui();

public:
    play();
    ~play();
    void play_game();
};

#endif // BSAI24056_PLAY_H