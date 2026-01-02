#ifndef BSAI24056_PLAYER_H
#define BSAI24056_PLAYER_H
#include"BSAI24056_utility.h"

class player
{
private:
	string name;
	turn color;
	bool is_checked;
public:

	int elo;
	player();
	player(string _name, turn _color, int _elo, bool _is_checked);
	string get_name();
	void set_name(const std::string& _name);
	void set_color(turn _color);
	void set_elo(int elo);
	int get_elo();
};

#endif 