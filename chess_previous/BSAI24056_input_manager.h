#ifndef BSAI24056_INPUT_MANAGER_H
#define BSAI24056_INPUT_MANAGER_H

#include"BSAI24056_utility.h"

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

#endif 