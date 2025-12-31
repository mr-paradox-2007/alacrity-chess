#ifndef ALACRITY_CHESS_INPUT_MANAGER_HPP
#define ALACRITY_CHESS_INPUT_MANAGER_HPP

#include "game/utility.hpp"

class input_manager
{
private:
	Vector2 virtual_mouse;
	Vector2 mouse;
	float scale;
public:
	void update_mouse();
	Vector2 get_v_m()const;
	Vector2 get_m()const;
	float get_scale()const;
};

#endif //ALACRITY_CHESS_INPUT_MANAGER_HPP
