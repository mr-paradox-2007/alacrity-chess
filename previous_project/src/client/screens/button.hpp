#ifndef ALACRITY_CHESS_BUTTON_HPP
#define ALACRITY_CHESS_BUTTON_HPP

#include "game/utility.hpp"

class button
{
private:
	Rectangle rect;
public:
	button(Rectangle _rect);
	void draw(Color _clr);
	bool is_being_hovered_on(Vector2 mouse_pos);
};

#endif //ALACRITY_CHESS_BUTTON_HPP
