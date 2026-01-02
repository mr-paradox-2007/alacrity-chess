#include "pawn.hpp"
#include "board.hpp"

pawn::pawn(position _pos, board* _b, turn _clr) : piece(_pos, _b, _clr)
{
}

void pawn::draw(float off_set, float square_size)
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

    DrawCylinder(pos3D, 0.34f, 0.28f, 0.20f, 16, piece_color);
    DrawCylinderWires(pos3D, 0.34f, 0.28f, 0.20f, 16, wire_color);

    Vector3 body_pos = { pos3D.x, pos3D.y + 0.20f, pos3D.z };
    DrawCylinder(body_pos, 0.25f, 0.20f, 0.40f, 16, piece_color);
    DrawCylinderWires(body_pos, 0.25f, 0.20f, 0.40f, 16, wire_color);

    Vector3 collar_pos = { pos3D.x, pos3D.y + 0.55f, pos3D.z };
    DrawCylinder(collar_pos, 0.24f, 0.24f, 0.05f, 16, accent_color);
    DrawCylinderWires(collar_pos, 0.24f, 0.24f, 0.05f, 16, wire_color);

    Vector3 head_pos = { pos3D.x, pos3D.y + 0.70f, pos3D.z };
    DrawSphere(head_pos, 0.20f, piece_color);
    DrawSphereWires(head_pos, 0.20f, 16, 16, wire_color);
}

bool pawn::is_legal_move(position _pos)
{
    if (clr == black)
    {
        if (_pos.y == pos.y && _pos.x == pos.x + 1 && b->get_piece(_pos) == nullptr)
            return true;

        if (_pos.y == pos.y && _pos.x == pos.x + 2 && pos.x == 1 &&
            b->get_piece(_pos) == nullptr && b->get_piece({ pos.x + 1, pos.y }) == nullptr)
            return true;

        if (_pos.x == pos.x + 1 &&
            (_pos.y == pos.y + 1 || _pos.y == pos.y - 1) &&
            b->get_piece(_pos) != nullptr && b->get_piece(_pos)->get_clr() != clr)
            return true;
    }
    else if (clr == white)
    {
        if (_pos.y == pos.y && _pos.x == pos.x - 1 && b->get_piece(_pos) == nullptr)
            return true;

        if (_pos.y == pos.y && _pos.x == pos.x - 2 && pos.x == 6 &&
            b->get_piece(_pos) == nullptr && b->get_piece({ pos.x - 1, pos.y }) == nullptr)
            return true;

        if (_pos.x == pos.x - 1 &&
            (_pos.y == pos.y + 1 || _pos.y == pos.y - 1) &&
            b->get_piece(_pos) != nullptr && b->get_piece(_pos)->get_clr() != clr)
            return true;
    }

    return false;
}
int pawn::get_type() const { return 0; }
