#include"utility.hpp"

bool position::operator==(position _p) const
{
    return x == _p.x && y == _p.y;
}
position::position(int _x, int _y)
{
    x = _x;
    y = _y;
}
