#include "BSAI24056_rook.h"
#include "BSAI24056_board.h"

rook::rook(position _pos, board* _b, turn _clr) : piece(_pos, _b, _clr)
{
}

void rook::draw(float off_set, float square_size)
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

    DrawCylinder(pos3D, 0.4f, 0.35f, 0.22f, 16, piece_color);
    DrawCylinderWires(pos3D, 0.4f, 0.35f, 0.22f, 16, wire_color);

    Vector3 body_pos = { pos3D.x, pos3D.y + 0.22f, pos3D.z };
    DrawCylinder(body_pos, 0.32f, 0.32f, 0.65f, 16, piece_color);
    DrawCylinderWires(body_pos, 0.32f, 0.32f, 0.65f, 16, wire_color);

    Vector3 ring_pos = { pos3D.x, pos3D.y + 0.75f, pos3D.z };
    DrawCylinder(ring_pos, 0.36f, 0.36f, 0.05f, 16, accent_color);
    DrawCylinderWires(ring_pos, 0.36f, 0.36f, 0.05f, 16, wire_color);

    Vector3 top_pos = { pos3D.x, pos3D.y + 0.87f, pos3D.z };
    DrawCylinder(top_pos, 0.38f, 0.38f, 0.16f, 16, piece_color);
    DrawCylinderWires(top_pos, 0.38f, 0.38f, 0.16f, 16, wire_color);

    for (int i = 0; i < 4; i++) 
    {
        float angle = i * PI / 2 + PI / 4;
        Vector3 cren_pos = {
            pos3D.x + 0.28f * cos(angle),
            pos3D.y + 1.03f,
            pos3D.z + 0.28f * sin(angle)
        };
        DrawCube(cren_pos, 0.12f, 0.18f, 0.12f, piece_color);
        DrawCubeWires(cren_pos, 0.12f, 0.18f, 0.12f, wire_color);
    }
}

bool rook::is_legal_move(position _pos)
{
    return (is_horizontal_move(_pos) && is_path_clear_horizontal(_pos)) ||
        (is_vertical_move(_pos) && is_path_clear_vertical(_pos));
}

int rook::get_type() const { return 1; }