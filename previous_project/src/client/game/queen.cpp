#include "queen.hpp"
#include "board.hpp"

queen::queen(position _pos, board* _b, turn _clr) : piece(_pos, _b, _clr)
{
}

void queen::draw(float off_set, float square_size)
{
    Vector3 pos3D =
    {
        off_set + pos.y * square_size,
        0.5f,
        off_set + pos.x * square_size
    };

    Color piece_color = (clr == white) ? Color{ 250, 245, 235, 220 } : Color{ 50, 30, 65, 220 };
    Color accent_color = (clr == white) ? Color{ 215, 200, 175, 180 } : Color{ 35, 20, 50, 180 };
    Color wire_color = (clr == white) ? Color{ 195, 185, 170, 255 } : Color{ 30, 15, 45, 255 };
    Color cross_color = (clr == white) ? Color{ 235, 205, 155, 255 } : Color{ 90, 40, 110, 255 };
    Color jewel_color = (clr == white) ? Color{ 215, 230, 255, 230 } : Color{ 180, 50, 90, 230 };

    DrawCylinder(pos3D, 0.4f, 0.32f, 0.22f, 16, piece_color);
    DrawCylinderWires(pos3D, 0.4f, 0.32f, 0.22f, 16, wire_color);

    Vector3 body_pos = { pos3D.x, pos3D.y + 0.22f, pos3D.z };
    DrawCylinder(body_pos, 0.32f, 0.24f, 0.55f, 16, piece_color);
    DrawCylinderWires(body_pos, 0.32f, 0.24f, 0.55f, 16, wire_color);

    Vector3 middle_ring = { pos3D.x, pos3D.y + 0.55f, pos3D.z };
    DrawCylinder(middle_ring, 0.28f, 0.28f, 0.04f, 16, accent_color);
    DrawCylinderWires(middle_ring, 0.28f, 0.28f, 0.04f, 16, wire_color);

    Vector3 neck_pos = { pos3D.x, pos3D.y + 0.72f, pos3D.z };
    DrawSphere(neck_pos, 0.17f, piece_color);
    DrawSphereWires(neck_pos, 0.17f, 16, 16, wire_color);

    Vector3 crown_pos = { pos3D.x, pos3D.y + 0.90f, pos3D.z };
    DrawCylinder(crown_pos, 0.22f, 0.26f, 0.16f, 16, piece_color);
    DrawCylinderWires(crown_pos, 0.22f, 0.26f, 0.16f, 16, wire_color);

    Vector3 crown_jewel = { pos3D.x, pos3D.y + 1.05f, pos3D.z };
    DrawSphere(crown_jewel, 0.07f, jewel_color);

    for (int i = 0; i < 8; i++) {
        float angle = i * PI / 4;
        Vector3 point_pos = {
            pos3D.x + 0.18f * cos(angle),
            pos3D.y + 1.06f,
            pos3D.z + 0.18f * sin(angle)
        };
        DrawSphere(point_pos, 0.06f, piece_color);
        DrawSphereWires(point_pos, 0.06f, 8, 8, wire_color);
    }
}

bool queen::is_legal_move(position _pos)
{
    return (is_horizontal_move(_pos) && is_path_clear_horizontal(_pos)) ||
        (is_vertical_move(_pos) && is_path_clear_vertical(_pos)) ||
        (is_diagonal_move(_pos) && is_path_clear_diagonal(_pos));
}

int queen::get_type() const { return 4; }
