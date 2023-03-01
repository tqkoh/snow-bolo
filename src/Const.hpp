#pragma once

#include <Siv3D.hpp>

#define DEV 0

#if DEV == 0
#define API_URL "wss://snowball-server.tqk.trap.show/api/ws"
#elif DEV == 1
#define API_URL "ws://dev.snowball-server.tqk.trap.show/api/ws"
#elif DEV == 2
#define API_URL "ws://localhost:3939/api/ws"
#else
#define API_URL ""
#endif

// #define FONT_PATH U"example/font/DotGothic16/DotGothic16-Regular.ttf"
#define FONT_PATH U"example/font/JF-Dot-k12x10.ttf"
#define FONT_SIZE_SMALL 12
#define FONT_SIZE_MEDIUM 20
#define FONT_SIZE_LARGE 36

const Size resolution(384, 216);
// const Size resolution(192, 108);
const int scaling = 4;

const ColorF backColor(U"#f0fefd");
const ColorF textColor1(U"#aaf0f3");
const ColorF textColor2(U"#ffd671");
