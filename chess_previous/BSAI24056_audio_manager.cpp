#include "BSAI24056_audio_manager.h"

audio_manager::audio_manager()
{
	InitAudioDevice();
	bgm = LoadMusicStream("assets/audio/bgm.mp3");
	menu_click = LoadSound("assets/audio/menu_click.mp3");
	menu_back = LoadSound("assets/audio/menu_back.mp3");
	not_good = LoadSound("assets/audio/not_good.mp3");
	sub_menu = LoadSound("assets/audio/sub_menu.mp3");
	move_white = LoadSound("assets/audio/move_white.mp3");
	move_black = LoadSound("assets/audio/move_black.mp3");
	capture = LoadSound("assets/audio/capture.mp3");
	promote = LoadSound("assets/audio/promote.mp3");
	check = LoadSound("assets/audio/check.mp3");
	checkmate = LoadSound("assets/audio/checkmate.mp3");
	draw = LoadSound("assets/audio/draw.mp3");
	castle = LoadSound("assets/audio/castle.mp3");
}
audio_manager::~audio_manager()
{
	UnloadMusicStream(bgm);
	UnloadSound(menu_click);
	UnloadSound(menu_back);
	UnloadSound(not_good);
	UnloadSound(sub_menu);
	UnloadSound(move_white);
	UnloadSound(move_black);
	UnloadSound(capture);
	UnloadSound(promote);
	UnloadSound(check);
	UnloadSound(checkmate);
	UnloadSound(draw);
	UnloadSound(castle);
	CloseAudioDevice();
}

void audio_manager::play_bgm()
{
	PlayMusicStream(bgm);
	SetMusicVolume(bgm, music_vol);
}
void audio_manager::stop_bgm()
{
	StopMusicStream(bgm);
}

void audio_manager::update()
{
	UpdateMusicStream(bgm);
}

void audio_manager::play_menu_click()
{
	PlaySound(menu_click);
}
void audio_manager::play_menu_back()
{
	PlaySound(menu_back);
}
void audio_manager::play_not_good()
{
	PlaySound(not_good);
}
void audio_manager::play_sub_menu()
{
	PlaySound(sub_menu);
}
void audio_manager::play_move_white()
{
	PlaySound(move_white);
}
void audio_manager::play_move_black()
{
	PlaySound(move_black);
}
void audio_manager::play_capture()
{
	PlaySound(capture);
}
void audio_manager::play_promote()
{
	PlaySound(promote);
}
void audio_manager::play_check()
{
	PlaySound(check);
}
void audio_manager::play_checkmate()
{
	PlaySound(checkmate);
}
void audio_manager::play_draw()
{
	PlaySound(draw);
}
void audio_manager::play_castle()
{
	PlaySound(castle);
}

void audio_manager::set_music_vol(float vol)
{
	music_vol = vol;
	SetMusicVolume(bgm, music_vol);
}
void audio_manager::set_sfx_vol(float vol)
{
	sfx_vol = vol;
	SetSoundVolume(menu_click, sfx_vol);
	SetSoundVolume(menu_back, sfx_vol);
	SetSoundVolume(not_good, sfx_vol);
	SetSoundVolume(sub_menu, sfx_vol);
	SetSoundVolume(move_white, sfx_vol);
	SetSoundVolume(move_black, sfx_vol);
	SetSoundVolume(capture, sfx_vol);
	SetSoundVolume(promote, sfx_vol);
	SetSoundVolume(check, sfx_vol);
	SetSoundVolume(checkmate, sfx_vol);
	SetSoundVolume(draw, sfx_vol);
	SetSoundVolume(castle, sfx_vol);
}