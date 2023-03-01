#pragma once

#include <Siv3D.hpp>
#include "../Const.hpp"
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

std::unique_ptr<Texture> prepareImage;

void init() {
	state = PREPARE;
	name.active = true;
	font = std::make_unique<Font>(FONT_SIZE_MEDIUM, FONT_PATH, FontStyle::Bitmap);
	prepareImage = std::make_unique<Texture>(U"assets/images/prepare.png");

	previnput[U"W"] = false;
	previnput[U"A"] = false;
	previnput[U"S"] = false;
	previnput[U"D"] = false;
	previnput[U"left"] = MouseL.pressed();
	previnput[U"right"] = MouseR.pressed();
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
				if(MouseL.pressed() || MouseR.pressed() ||
					 KeyW.pressed() != previnput[U"W"].get<bool>() ||
					 KeyA.pressed() != previnput[U"A"].get<bool>() ||
					 KeyS.pressed() != previnput[U"S"].get<bool>() ||
					 KeyD.pressed() != previnput[U"D"].get<bool>() ||
					 MouseL.pressed() != previnput[U"left"].get<bool>() ||
					 MouseR.pressed() != previnput[U"right"].get<bool>()) {
					ws.SendText(json.formatUTF8Minimum());
					previnput = input;

					std::cout << std::time(nullptr) << std::endl;
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
			SimpleGUI::TextBox(name, Vec2(-1000, -1000), 100, 12);
			prepareImage->draw(0, 0);
			(*font)(name.text).drawAt(Vec2(300, center.y), textColor1);
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
