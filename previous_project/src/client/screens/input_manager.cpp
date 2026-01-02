#include "input_manager.hpp"
#include "game/utility.hpp" // For internal_width, internal_height

void input_manager::update_mouse()
{
	scale = fminf((float)GetScreenWidth()/internal_width,(float)GetScreenHeight() / internal_height);

	mouse = GetMousePosition();
	virtual_mouse =
	{
		(mouse.x - (GetScreenWidth() - internal_width * scale) * 0.5f) / scale,
		(mouse.y - (GetScreenHeight() - internal_height * scale) * 0.5f) / scale
	};
	virtual_mouse=Vector2Clamp(virtual_mouse,{0,0},{(float)internal_width,(float)internal_height});
}
Vector2 input_manager::get_v_m()const
{
	return virtual_mouse;
}
Vector2 input_manager::get_m()const
{
	return mouse;
}
float input_manager::get_scale()const
{
	return scale;
}
