#include "button.hpp"

button::button(Rectangle _rect) :rect(_rect)
{
}
void button::draw(Color _clr)
{
	DrawRectangleRec(rect, _clr);
}
bool button::is_being_hovered_on(Vector2 mouse_pos)
{
	return CheckCollisionPointRec(mouse_pos, rect);
}
