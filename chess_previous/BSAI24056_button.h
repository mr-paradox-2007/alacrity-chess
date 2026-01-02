#ifndef BSAI24056_BUTTON_H
#define BSAI24056_BUTTON_H

#include"BSAI24056_utility.h"

class button
{
private:
	Rectangle rect;
public:
	button(Rectangle _rect);
	void draw(Color _clr);
	bool is_being_hovered_on(Vector2 mouse_pos);
};

#endif //BSAI24056_BUTTON_H