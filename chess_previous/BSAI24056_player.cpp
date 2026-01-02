#include "BSAI24056_player.h"

player::player() : name(""), color(white), elo(1000), is_checked(false)
{
}
player::player(string _name, turn _color, int _elo, bool _is_checked) :name(_name), color(_color), elo(_elo), is_checked(_is_checked)
{

}
string player::get_name()
{
	return this->name;
}
void player::set_name(const string& _name)
{
	name = _name;
}
void player::set_color(turn _color)
{
	color = _color;
}
void player::set_elo(int elo)
{
	this->elo = elo;
}
int player::get_elo()
{
	return elo;
}