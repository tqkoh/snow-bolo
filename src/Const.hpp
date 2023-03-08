#pragma once

#include <Siv3D.hpp>

#define DEV 0

#if DEV == 0
#define API_URL "wss://tqk.trap.show/snowball-server/api/ws"
#elif DEV == 1
#define API_URL "ws://dev.snowball-server.tqk.trap.show/api/ws"
#elif DEV == 2
#define API_URL "ws://localhost:3939/api/ws"
#else
#define API_URL ""
#endif

#define SEND_INPUT_PER 6

#define MAX_V 4
#define V_K 0.2

#define RADIUS_M 50
#define RADIUS_V 0.0001

#define FONT_PATH U"assets/fonts/JF-Dot-k12x10.ttf"
#define FONT_SIZE_SMALL 10
#define FONT_SIZE_MEDIUM 20
#define FONT_SIZE_LARGE 30

#define MAP_HEIGHT 1728
#define MAP_WIDTH 1728
#define MAP_MARGIN 324

const Size resolution(384, 216);
const int scaling = 8;

const ColorF backColor(U"#f0fefd");
const ColorF textColor1(U"#aaf0f3");
const ColorF textColor2(U"#ffd671");
const ColorF paintColor1(U"#d6f7f9");
const ColorF paintColor2(U"#fef8ca");
const ColorF ballColor(U"#feffff");
const ColorF damageColor(ColorF(U"#ff2222"), 0.3);
const ColorF recoverColor(ColorF(U"#22ff22"), 0.3);
const ColorF shadowColor(0, .15);

#define GAME_KATASA_Y 175
#define GAME_KATASA_DEKASA_X 60
#define GAME_DEKASA_Y 198
#define GAME_KATASA_DEKASA_HEIGHT 10

#define ANIMATION_DAMAGE_LENGTH 30
