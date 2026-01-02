#ifndef ALACRITY_CHESS_CAMERA_MANAGER_HPP
#define ALACRITY_CHESS_CAMERA_MANAGER_HPP

#include "game/utility.hpp"

class camera_manager
{
private:
	Camera camera;
	float current_angle_y, current_angle_x;
	float target_angle_y, target_angle_x;
	Vector3 current_pos;
	Vector3 target_pos;
	bool flipped;
public:
	camera_manager(); // Added constructor
	void reset();
	void handle_camera_movement();
	Camera get_camera()const;
	void flip_camera();
};

#endif //ALACRITY_CHESS_CAMERA_MANAGER_HPP
