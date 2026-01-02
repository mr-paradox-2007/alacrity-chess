#ifndef BSAI24056_UTILITY_H
#define BSAI24056_UTILITY_H

#include <iostream>
#include <string>
#include <vector>
#include<conio.h>
#include "raylib.h"
#include "raymath.h"
#include <fstream>

using namespace std;

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

    bool operator==(position _p);

    position()
    {
        x = 0, y = 0;
    }
    position(int _x, int _y);
};

#endif 