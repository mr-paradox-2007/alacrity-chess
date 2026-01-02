#ifndef BSAI24056_AUDIO_MANAGER_H
#define BSAI24056_AUDIO_MANAGER_H

#include"BSAI24056_utility.h"

class audio_manager
{
private:
	Music bgm;

	Sound menu_click;
	Sound menu_back;
	Sound not_good;
	Sound sub_menu;
	Sound move_white;
	Sound move_black;
	Sound capture;
	Sound promote;
	Sound check;
	Sound checkmate;
	Sound draw;
	Sound castle;

	float music_vol = 0.5f;
	float sfx_vol = 0.7f;

public:
	audio_manager();
	~audio_manager();

	void play_bgm();
	void stop_bgm();

	void update();

	void play_menu_click();
	void play_menu_back();
	void play_not_good();
	void play_sub_menu();
	void play_move_white();
	void play_move_black();
	void play_capture();
	void play_promote();
	void play_check();
	void play_checkmate();
	void play_draw();
	void play_castle();

	void set_music_vol(float vol);
	void set_sfx_vol(float vol);
};

#endif //BSAI24056_AUDIO_MANAGER_H