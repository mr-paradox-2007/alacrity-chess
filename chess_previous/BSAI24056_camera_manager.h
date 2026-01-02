#ifndef BSAI24056_CAMERA_MANAGER_H
#define BSAI24056_CAMERA_MANAGER_H

#include"BSAI24056_utility.h"

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
	void reset();
	void handle_camera_movement();
	Camera get_camera()const;
	void flip_camera();
};

#endif //BSAI24056_CAMERA_MANAGER_H