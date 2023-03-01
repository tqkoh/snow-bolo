#pragma once

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

// #define FONT_PATH U"example/font/DotGothic16/DotGothic16-Regular.ttf"
#define FONT_PATH U"example/font/PixelMplus10-Regular.ttf"

const Size resolution(384, 216);
const int scaling = 8;

#include <Siv3D.hpp>
#include "lib/WebSocket.hpp"

enum GameState {
	PREPARE,
	PLAYING,
	DEAD,

	GameState_NUM
};

namespace Game {

WebSocket ws(API_URL);
GameState state = PREPARE;
TextEditState name;
Point center(resolution / 2);

std::unique_ptr<Font> font;
JSON previnput;

void init() {
	state = PREPARE;
	name.active = true;
	font = std::make_unique<Font>(20, FONT_PATH, FontStyle::Bitmap);
}
int update() {
	switch(state) {
		case PREPARE:
			name.active = true;

			if(KeyEnter.down()) {
				JSON json, input;
				input[U"name"] = name.text;
				json[U"Method"] = U"join";
				json[U"Args"] = input;
				ws.SendText(json.formatUTF8Minimum());
				state = PLAYING;
			}
			break;
		case PLAYING:
			while(ws.hasReceivedText()) {
				auto received = Unicode::FromUTF8(ws.getReceivedTextAndPopFromBuffer());
				JSON json = JSON::Parse(received);
			}
			// send input
			{
				JSON json, input;
				auto p = Cursor::Pos();
				input[U"W"] = KeyW.pressed();
				input[U"A"] = KeyA.pressed();
				input[U"S"] = KeyS.pressed();
				input[U"D"] = KeyD.pressed();
				input[U"left"] = MouseL.pressed();
				input[U"right"] = MouseR.pressed();
				input[U"dy"] = p.y - center.y;
				input[U"dx"] = p.x - center.x;

				json[U"Method"] = U"input";
				json[U"Args"] = input;

				// look at the md #2
				if(input[U"left"] || input[U"right"]) {
					ws.SendText(json.formatUTF8Minimum());
					previnput = input;
				} else {
					for(auto e : input) {
						if(e.key == U"dy" || e.key == U"dx") {
							continue;
						}

						if(previnput[e.key] != e.value) {
							ws.SendText(json.formatUTF8Minimum());
							previnput = input;
							break;
						}
					}
				}
			}
			break;
		case DEAD:
			break;
		default:
			state = PREPARE;
			break;
	}
	if(KeyN.down()) {	 // tmp
		return 1;
	}

	if(KeyC.down()) {
		JSON json;
		json[U"cursor"] = Cursor::Pos();
		ws.SendText(json.formatUTF8Minimum());
	}

	if(KeyA.down()) {
		JSON json;
		json[U"message"] = U"Key A!";
		ws.SendText(json.formatUTF8Minimum());
	}
	return 0;
}
void draw() {
	// (*font)(U"Hello, Siv3D!🚀").drawAt(Scene::Center() / 11, Palette::Green);
	switch(state) {
		case PREPARE:

			SimpleGUI::TextBox(name, Vec2(-1000, -1000), 100);
			Print << name.text;
			(*font)(name.text).drawAt(Scene::Center() / scaling, Palette::Black);
			break;
		case PLAYING:
			Circle{Cursor::Pos() / scaling, 20}.draw(ColorF{1, 1, 0, 0.5});
			break;
		case DEAD:
			break;
		default:
			break;
	}
}

}	 // namespace Game

// namespace Game {

// WebSocket ws(API_URL);
// GameState state = PREPARE;
// TextEditState name;
// Point center(resolution / 2);

// Font font(30,
// 					U"example/font/DotGothic16/DotGothic16-Regular.ttf",
// 					FontStyle::Bitmap);

// void init() {
// 	state = PREPARE;
// 	name.active = true;
// }
// int update() {
// 	switch(state) {
// 		case PREPARE:
// 			name.active = true;

// 			if(KeyEnter.down()) {
// 				JSON json, args;
// 				args[U"name"] = name.text;
// 				json[U"Method"] = U"join";
// 				json[U"Args"] = args;
// 				ws.SendText(json.formatUTF8Minimum());
// 				state = PLAYING;
// 			}
// 			break;
// 		case PLAYING:
// 			while(ws.hasReceivedText()) {
// 				auto received =
// Unicode::FromUTF8(ws.getReceivedTextAndPopFromBuffer()); 				JSON json =
// JSON::Parse(received);
// 			}
// 			// send input
// 			{
// 				JSON json, args;
// 				auto p = Cursor::Pos();
// 				args[U"W"] = KeyW.pressed();
// 				args[U"A"] = KeyA.pressed();
// 				args[U"S"] = KeyS.pressed();
// 				args[U"D"] = KeyD.pressed();
// 				args[U"left"] = MouseL.pressed();
// 				args[U"right"] = MouseR.pressed();
// 				args[U"dy"] = p.y - center.y;
// 				args[U"dx"] = p.x - center.x;

// 				json[U"Method"] = U"input";
// 				json[U"Args"] = args;
// 				ws.SendText(json.formatUTF8Minimum());
// 			}
// 			break;
// 		case DEAD:
// 			break;
// 		default:
// 			state = PREPARE;
// 			break;
// 	}
// 	if(KeyN.down()) {	 // tmp
// 		return 1;
// 	}

// 	if(KeyC.down()) {
// 		JSON json;
// 		json[U"cursor"] = Cursor::Pos();
// 		ws.SendText(json.formatUTF8Minimum());
// 	}

// 	if(KeyA.down()) {
// 		JSON json;
// 		json[U"message"] = U"Key A!";
// 		ws.SendText(json.formatUTF8Minimum());
// 	}
// 	return 0;
// }
// void draw() {
// 	font(U"Hello, Siv3D!🚀").drawAt(Scene::Center() / 11, Palette::Green);
// 	switch(state) {
// 		case PREPARE:

// 			SimpleGUI::TextBox(name, Vec2(100, 10), 100);
// 			Print << name.text;
// 			font(name.text).drawAt(Scene::Center() / 4, Palette::Black);
// 			break;
// 		case PLAYING:
// 			Circle{Cursor::Pos() / scaling, 20}.draw(ColorF{1, 1, 0, 0.5});
// 			break;
// 		case DEAD:
// 			break;
// 		default:
// 			break;
// 	}
// }

// }	 // namespace Game
