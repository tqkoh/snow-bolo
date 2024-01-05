#pragma once

#include <Siv3D.hpp>

#define DEV 2

#if DEV == 0
#define API_URL "wss://tqk.trap.show/snow-bolo-server/api/ws"
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
#define FONT_SIZE_LARGE 40

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

#define CHAT_REMAIN (60 * 5)
#define LEADERBOARD_X 275

#define HISTORY_MIN_INTERVAL 30
#define HISTORY_MIN_APPEND_W 2
#define HISTORY_Y 62
#define HISTORY_X 150
#define HISTORY_H (166 - 62)
#define HISTORY_W (306 - 150)

#define THEME_LOOP_BEGIN 88200 * 19 + 1994900 - 1940400
#define THEME_LOOP_END 88200 * 19 + 1994900 - 1940400 + 88200 * 12

#define TWEET_URL(maxMass, bestRank, kills)                                                                                                              \
	U"https://twitter.com/intent/tweet?url=https://tqk.blue/snow-bolo/&text=snow-bolo で遊んだよ！\%0A最大デカさ: {}\%0A最高順位: {}\%0Aキル: {}\%0A"_fmt( \
			(maxMass), (bestRank), (kills))
