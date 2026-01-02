#include "bishop.hpp"
#include "board.hpp"

bishop::bishop(position _pos, board* _b, turn _clr) : piece(_pos, _b, _clr)
{
}

void bishop::draw(float off_set, float square_size)
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

    DrawCylinder(pos3D, 0.35f, 0.28f, 0.22f, 16, piece_color);
    DrawCylinderWires(pos3D, 0.35f, 0.28f, 0.22f, 16, wire_color);

    Vector3 body_pos = { pos3D.x, pos3D.y + 0.22f, pos3D.z };
    DrawCylinder(body_pos, 0.26f, 0.22f, 0.45f, 16, piece_color);
    DrawCylinderWires(body_pos, 0.26f, 0.22f, 0.45f, 16, wire_color);

    Vector3 collar_pos = { pos3D.x, pos3D.y + 0.55f, pos3D.z };
    DrawCylinder(collar_pos, 0.28f, 0.28f, 0.05f, 16, accent_color);
    DrawCylinderWires(collar_pos, 0.28f, 0.28f, 0.05f, 16, wire_color);

    Vector3 head_pos = { pos3D.x, pos3D.y + 0.67f, pos3D.z };
    DrawCylinder(head_pos, 0.22f, 0.08f, 0.32f, 16, piece_color);
    DrawCylinderWires(head_pos, 0.22f, 0.08f, 0.32f, 16, wire_color);

    Vector3 top_pos = { pos3D.x, pos3D.y + 0.99f, pos3D.z };
    DrawSphere(top_pos, 0.10f, piece_color);
    DrawSphereWires(top_pos, 0.10f, 16, 16, wire_color);

    Vector3 cross_base = { pos3D.x, pos3D.y + 1.09f, pos3D.z };
    DrawCube(cross_base, 0.04f, 0.08f, 0.04f, cross_color);

    Vector3 cross_top = { pos3D.x, pos3D.y + 1.09f, pos3D.z };
    DrawCube(cross_top, 0.12f, 0.04f, 0.04f, cross_color);
}

bool bishop::is_legal_move(position _pos)
{
    return (is_diagonal_move(_pos) && is_path_clear_diagonal(_pos));
}

int bishop::get_type() const { return 3; }
