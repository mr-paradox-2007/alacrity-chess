#include"BSAI24056_piece.h"

#include "BSAI24056_board.h"

int piece::abs(int x)
{
	return (x < 0) ? -x : x;
}

piece::piece(position _pos, board* _b, turn _clr) : pos(_pos), b(_b), clr(_clr)
{
}
turn piece::get_clr()
{
	return clr;
}
position piece::get_pos()
{
	return pos;
}
board* piece::get_b()
{
	return b;
}
void piece::move(position _pos)
{
	pos = _pos;
}
bool piece::is_horizontal_move(position _pos)
{
	return pos.y == _pos.y && pos.x != _pos.x;
}
bool piece::is_vertical_move(position _pos)
{
	return pos.x == _pos.x && pos.y != _pos.y;
}
bool piece::is_diagonal_move(position _pos)
{
	return abs(pos.x - _pos.x) == abs(pos.y - _pos.y);
}
bool piece::is_path_clear_horizontal(position _pos)
{
	if (!is_horizontal_move(_pos))
		return true;
	int dir = (_pos.x > pos.x) ? 1 : -1;
	for (int x = pos.x + dir; x != _pos.x; x += dir)
	{
		if (b->get_piece({ x, pos.y }) != nullptr)
			return false;
	}
	return true;
}
bool piece::is_path_clear_vertical(position _pos)
{
	if (!is_vertical_move(_pos))
		return false;

	int dir = (_pos.y > pos.y) ? 1 : -1;
	for (int y = pos.y + dir; y != _pos.y; y += dir)
	{
		if (b->get_piece({ pos.x, y }) != nullptr)
			return false;
	}
	return true;
}
bool piece::is_path_clear_diagonal(position _pos)
{
	if (!is_diagonal_move(_pos))
		return false;

	int dx = (_pos.x > pos.x) ? 1 : -1;
	int dy = (_pos.y > pos.y) ? 1 : -1;

	for (int x = pos.x + dx, y = pos.y + dy; x != _pos.x && y != _pos.y; x += dx, y += dy)
	{
		if (b->get_piece({ x,y }) != nullptr)
			return false;
	}

	return true;
}

piece::~piece()
{
	if (b != nullptr)
		delete b;
}