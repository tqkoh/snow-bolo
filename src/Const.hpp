#pragma once

#include <Siv3D.hpp>

#define DEV 2

#if DEV == 0
#define API_URL "wss://snowball-server.tqk.trap.show/api/ws"
#elif DEV == 1
#define API_URL "ws://dev.snowball-server.tqk.trap.show/api/ws"
#elif DEV == 2
#define API_URL "ws://localhost:3939/api/ws"
#else
#define API_URL ""
#endif

#define SEND_INPUT_PER 6

#define V_MAX 4
#define V_K 0.2

#define FONT_PATH U"assets/fonts/JF-Dot-k12x10.ttf"
#define FONT_SIZE_SMALL 12
#define FONT_SIZE_MEDIUM 20
#define FONT_SIZE_LARGE 36

const Size resolution(384, 216);
const int scaling = 8;

const ColorF backColor(U"#f0fefd");
const ColorF textColor1(U"#aaf0f3");
const ColorF textColor2(U"#ffd671");
