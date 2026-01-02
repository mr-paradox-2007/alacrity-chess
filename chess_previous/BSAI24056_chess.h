#ifndef BSAI24056_CHESS_H
#define BSAI24056_CHESS_H

#include"BSAI24056_utility.h"
#include"BSAI24056_audio_manager.h"
#include"BSAI24056_button.h"
#include"BSAI24056_input_manager.h"
#include"BSAI24056_board.h"
#include"BSAI24056_camera_manager.h"
#include"BSAI24056_game_player.h"

class chess
{
private:

	RenderTexture2D display_screen;
	
	audio_manager audio;
	input_manager input;
	game_player game;

	bool is_main_menu;
	bool is_settings;
	bool is_paused;
	bool is_playing;
	bool exit_game;
	bool game_loaded;
	bool music_on;

	void setup_window();
	void main_loop();

	void main_menu();
	void settings();

	void handle_button_hover_draw(button& btn, Vector2 mouse_pos);
	void handle_button_hover_draw(button& btn, Vector2 mouse_pos, Color hover_color);

public:
	
	chess();
	~chess();
	void play();

};

#endif //BSAI24056_CHESS_H