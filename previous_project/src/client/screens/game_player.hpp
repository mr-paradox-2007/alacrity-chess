#ifndef ALACRITY_CHESS_GAME_PLAYER_HPP
#define ALACRITY_CHESS_GAME_PLAYER_HPP

#include "game/utility.hpp"
#include "game/board.hpp"
#include "camera_manager.hpp"
#include "input_manager.hpp"
#include "audio_manager.hpp"
#include "button.hpp"
#include "game/move_history.hpp"

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
	std::vector<position> valid_moves;

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
    game_player(); // Added constructor
	void reset(bool has_loaded);
	bool play(audio_manager& audio);
};

#endif // ALACRITY_CHESS_GAME_PLAYER_HPP
