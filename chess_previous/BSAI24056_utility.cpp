#include"BSAI24056_utility.h"

bool position::operator==(position _p)
{
    return x == _p.x && y == _p.y;
}
position::position(int _x, int _y)
{
    x = _x;
    y = _y;
}