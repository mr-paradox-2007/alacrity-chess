#include "BSAI24056_knight.h"
#include "BSAI24056_board.h"

knight::knight(position _pos, board* _b, turn _clr) : piece(_pos, _b, _clr)
{
}

void knight::draw(float off_set, float square_size)
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

    DrawCylinder(pos3D, 0.38f, 0.30f, 0.22f, 16, piece_color);
    DrawCylinderWires(pos3D, 0.38f, 0.30f, 0.22f, 16, wire_color);

    Vector3 body_pos = { pos3D.x, pos3D.y + 0.22f, pos3D.z };
    DrawCylinder(body_pos, 0.26f, 0.22f, 0.35f, 16, piece_color);
    DrawCylinderWires(body_pos, 0.26f, 0.22f, 0.35f, 16, wire_color);

    Vector3 collar_pos = { pos3D.x, pos3D.y + 0.50f, pos3D.z };
    DrawCylinder(collar_pos, 0.26f, 0.26f, 0.05f, 16, accent_color);
    DrawCylinderWires(collar_pos, 0.26f, 0.26f, 0.05f, 16, wire_color);

    Vector3 neck_pos = { pos3D.x + 0.12f, pos3D.y + 0.57f, pos3D.z };
    DrawCylinder(neck_pos, 0.17f, 0.12f, 0.35f, 16, piece_color);
    DrawCylinderWires(neck_pos, 0.17f, 0.12f, 0.35f, 16, wire_color);

    Vector3 head_base = { pos3D.x + 0.24f, pos3D.y + 0.72f, pos3D.z };
    DrawSphere(head_base, 0.20f, piece_color);
    DrawSphereWires(head_base, 0.20f, 12, 12, wire_color);

    Vector3 head_extend = { pos3D.x + 0.28f, pos3D.y + 0.72f, pos3D.z };
    DrawSphere(head_extend, 0.18f, piece_color);

    Vector3 muzzle_pos = { pos3D.x + 0.34f, pos3D.y + 0.72f, pos3D.z };
    DrawSphere(muzzle_pos, 0.12f, piece_color);
    DrawSphereWires(muzzle_pos, 0.12f, 8, 8, wire_color);

    Vector3 ear_pos = { pos3D.x + 0.14f, pos3D.y + 0.90f, pos3D.z };
    DrawCylinder(ear_pos, 0.06f, 0.01f, 0.15f, 8, accent_color);
    DrawCylinderWires(ear_pos, 0.06f, 0.01f, 0.15f, 8, wire_color);

    Vector3 mane_pos = { pos3D.x + 0.05f, pos3D.y + 0.80f, pos3D.z };
    DrawSphere(mane_pos, 0.12f, accent_color);
    DrawSphereWires(mane_pos, 0.12f, 8, 8, wire_color);

    Vector3 mane_detail = { pos3D.x + 0.07f, pos3D.y + 0.73f, pos3D.z };
    DrawSphere(mane_detail, 0.09f, accent_color);

}

bool knight::is_legal_move(position _pos)
{
    int x_diff = abs(pos.x - _pos.x);
    int y_diff = abs(pos.y - _pos.y);
    return (x_diff == 2 && y_diff == 1) || (x_diff == 1 && y_diff == 2);
}

int knight::get_type() const { return 2; }