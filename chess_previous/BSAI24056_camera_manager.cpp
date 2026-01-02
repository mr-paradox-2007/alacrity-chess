#include "BSAI24056_camera_manager.h"

void camera_manager::reset() 
{
    flipped = false;
    current_angle_x = target_angle_x = 0.0f;
    current_angle_y = target_angle_y = 0.0f;
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void camera_manager::handle_camera_movement() 
{
    if (IsKeyDown(KEY_UP))    target_angle_y += 0.01f;
    if (IsKeyDown(KEY_DOWN))  target_angle_y -= 0.01f;
    target_angle_y = Clamp(target_angle_y, -PI * 0.05f, PI * 0.4f);
    float dt = GetFrameTime();
    float speed = 4.0f;
    float t = Clamp(dt * speed, 0.0f, 1.0f);
    current_angle_x += (target_angle_x - current_angle_x) * t;
    current_angle_y += (target_angle_y - current_angle_y) * t;
    float r = 10.0f;
    float ch = 2.0f;
    current_pos = {
        r * cosf(current_angle_y) * sinf(current_angle_x),
        r * sinf(current_angle_y) + ch,
        r * cosf(current_angle_y) * cosf(current_angle_x)
    };
    camera.position = current_pos;
}

Camera camera_manager::get_camera() const
{
    return camera;
}

void camera_manager::flip_camera() 
{
    flipped = !flipped;
    target_angle_x += PI;
}
