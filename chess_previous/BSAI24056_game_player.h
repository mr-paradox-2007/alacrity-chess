#ifndef BSAI24056_GAME_PLAYER_H
#define BSAI24056_GAME_PLAYER_H

#include"BSAI24056_utility.h"
#include"BSAI24056_board.h"
#include"BSAI24056_camera_manager.h"
#include"BSAI24056_input_manager.h"
#include"BSAI24056_audio_manager.h"
#include"BSAI24056_button.h"
#include"BSAI24056_move_history.h"

class game_player
{
private:
	board b;
	turn t;
	turn saved_turn;
	
	camera_manager cam;
	input_manager input;
	move_history history;

	position selected_pos;
	position pos;
	position promotion_position;
	vector<position> valid_moves;

	bool is_paused;
	bool piece_selected;
	bool promotion_pending;
	bool quit;
	bool replay_mode;

	position get_board_position_from_mouse();
	void handle_piece_selection(position pos);
	void calculate_valid_moves();
	void handle_piece_move(position pos,audio_manager& audio);
	void draw_board_highlights();
	void game_paused();
	void handle_button_hover_draw(button& btn, Vector2 mouse_pos, Color hover_color);
	void display_game_over();
	void handle_pawn_promotion();
	int draw_promotion_ui();
	chess_move create_move_record(position from, position to);
	void undo_last_move();
	void redo_move();
	void enter_replay_mode();
	void exit_replay_mode();
	void replay_step_forward();
	void replay_step_backward();
	void reverse_move(const chess_move& move);
	void replay_single_move(const chess_move& move);
	void rebuild_to_current_position();
	void restore_to_current_position();
	void draw_replay_controls();

public:
	void reset(bool has_loaded);
	bool play(audio_manager& audio);
};

#endif // BSAI24056_GAME_PLAYER_H