#include "king.hpp"
#include "board.hpp"

king::king(position _pos, board* _b, turn _clr) : piece(_pos, _b, _clr)
{
}

void king::draw(float off_set, float square_size)
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
    DrawCylinder(body_pos, 0.30f, 0.25f, 0.55f, 16, piece_color);
    DrawCylinderWires(body_pos, 0.30f, 0.25f, 0.55f, 16, wire_color);

    Vector3 middle_ring = { pos3D.x, pos3D.y + 0.50f, pos3D.z };
    DrawCylinder(middle_ring, 0.32f, 0.32f, 0.05f, 16, accent_color);
    DrawCylinderWires(middle_ring, 0.32f, 0.32f, 0.05f, 16, wire_color);

    Vector3 upper_body = { pos3D.x, pos3D.y + 0.65f, pos3D.z };
    DrawCylinder(upper_body, 0.25f, 0.22f, 0.20f, 16, piece_color);
    DrawCylinderWires(upper_body, 0.25f, 0.22f, 0.20f, 16, wire_color);

    Vector3 neck_pos = { pos3D.x, pos3D.y + 0.75f, pos3D.z };
    DrawSphere(neck_pos, 0.18f, piece_color);
    DrawSphereWires(neck_pos, 0.18f, 16, 16, wire_color);

    Vector3 crown_pos = { pos3D.x, pos3D.y + 0.90f, pos3D.z };
    DrawCylinder(crown_pos, 0.24f, 0.28f, 0.18f, 16, piece_color);
    DrawCylinderWires(crown_pos, 0.24f, 0.28f, 0.18f, 16, wire_color);

    for (int i = 0; i < 6; i++) {
        float angle = i * PI / 3;
        Vector3 point_pos = {
            pos3D.x + 0.20f * cos(angle),
            pos3D.y + 1.08f,
            pos3D.z + 0.20f * sin(angle)
        };
        DrawSphere(point_pos, 0.06f, jewel_color);
    }

    Vector3 cross_base = { pos3D.x, pos3D.y + 1.08f, pos3D.z };
    DrawCube(cross_base, 0.05f, 0.28f, 0.05f, cross_color);

    Vector3 cross_arm = { pos3D.x, pos3D.y + 1.20f, pos3D.z };
    DrawCube(cross_arm, 0.18f, 0.05f, 0.05f, cross_color);
}

bool king::is_legal_move(position _pos)
{
    int x_diff = abs(pos.x - _pos.x);
    int y_diff = abs(pos.y - _pos.y);

    if (x_diff <= 1 && y_diff <= 1 && (x_diff > 0 || y_diff > 0))
    {
        return true;
    }

    if (is_castling_move(_pos))
    {
        bool kingside = (_pos.y > pos.y);
        if (kingside) {
            return b->can_castle_kingside(clr);
        }
        else {
            return b->can_castle_queenside(clr);
        }
    }

    return false;
}

bool king::is_castling_move(position _pos)
{

    if (pos.x == _pos.x && abs(pos.y - _pos.y) == 2)
    {
        return true;
    }
    return false;
}

int king::get_type() const { return 5; }
