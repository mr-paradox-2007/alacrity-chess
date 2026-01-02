#ifndef ALACRITY_CHESS_UTILITY_HPP
#define ALACRITY_CHESS_UTILITY_HPP

#include <iostream>
#include <string>
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include <fstream>

const int internal_width = 1920;
const int internal_height = 1080;
const int screen_width = 1280;
const int screen_height = 720;
const float sq = 1.0f;
const int   N = 8;
const float off = -(N * sq) / 2 + sq / 2;

enum turn
{
    black = 0,
    white = 1
};
struct position
{
    int x, y;

    bool operator==(position _p) const;

    position()
    {
        x = 0, y = 0;
    }
    position(int _x, int _y);
};

#endif // ALACRITY_CHESS_UTILITY_HPP
