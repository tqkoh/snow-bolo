#pragma once

#include "../Const.hpp"
#include "../Global.hpp"
#include "lib/WebSocket.hpp"

#define MOD(n, m) (((n) % (m) + (m)) % (m))

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

std::unique_ptr<Font> fontSmall;
std::unique_ptr<Font> fontMedium;
JSON previnput;

std::unique_ptr<Texture> backImage;
std::unique_ptr<Texture> ballImage;
std::unique_ptr<Texture> prepareImage;

JSON lastUpdate;

float oY = 0, oX = 0, oVY = 0, oVX = 0;
String id;

void init() {
	state = PREPARE;
	name.active = true;
	fontSmall =
			std::make_unique<Font>(FONT_SIZE_SMALL, FONT_PATH, FontStyle::Bitmap);
	fontMedium =
			std::make_unique<Font>(FONT_SIZE_MEDIUM, FONT_PATH, FontStyle::Bitmap);
	backImage = std::make_unique<Texture>(U"assets/images/back.png");
	ballImage = std::make_unique<Texture>(U"assets/images/ball.png");
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
		case PREPARE: {
			name.active = true;

			if(KeyEnter.down()) {
				JSON json, input;
				input[U"name"] = name.text;
				json[U"Method"] = U"join";
				json[U"Args"] = input;
				ws.SendText(json.formatUTF8Minimum());
				state = PLAYING;
				name.active = false;
				SimpleGUI::TextBox(name, Vec2(-1000, -1000), 100, 12, false);
			}
			break;
		}
		case PLAYING: {
			String received;
			while(ws.hasReceivedText()) {
				received = Unicode::FromUTF8(ws.getReceivedTextAndPopFromBuffer());
				JSON json = JSON::Parse(received);

				// printf("received: %s\n", received.narrow().c_str());
				if(json[U"method"] == U"joinAccepted") {
					id = json[U"args"][U"id"].get<String>();
				}
				if(json[U"method"] == U"update") {
					lastUpdate = json[U"args"];
					lastUpdate[U"timestamp"] = frame;
				}
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
				if(MouseL.pressed() && frame % SEND_INPUT_PER == 0 ||
					 MouseR.pressed() && frame % SEND_INPUT_PER == 0 ||
					 KeyW.pressed() != previnput[U"W"].get<bool>() ||
					 KeyA.pressed() != previnput[U"A"].get<bool>() ||
					 KeyS.pressed() != previnput[U"S"].get<bool>() ||
					 KeyD.pressed() != previnput[U"D"].get<bool>() ||
					 MouseL.pressed() != previnput[U"left"].get<bool>() ||
					 MouseR.pressed() != previnput[U"right"].get<bool>()) {
					ws.SendText(json.formatUTF8Minimum());
					previnput = input;

					// printf("time: %lld", std::time(nullptr));
				}
			}
			break;
		}
		case DEAD: {
			break;
		}

		default: {
			state = PREPARE;
			break;
		}
	}
	if(KeyN.down()) {	 // tmp
		return 1;
	}
	return 0;
}
void draw() {
	// (*font)(U"Hello, Siv3D!🚀").drawAt(Scene::Center() / 11, Palette::Green);
	switch(state) {
		case PREPARE: {
			SimpleGUI::TextBox(name, Vec2(-1000, -1000), 100, 12);
			prepareImage->draw(0, 0);
			(*fontMedium)(name.text).drawAt(Vec2(300, center.y), textColor1);
			break;
		}
		case PLAYING: {
			// update origin
			if(lastUpdate.hasElement(U"users")) {
				for(int i = 0; i < lastUpdate[U"users"].size(); i++) {
					if(lastUpdate[U"users"][i][U"id"].get<String>() == id) {
						oVY = lastUpdate[U"users"][i][U"vy"].get<float>();
						oVX = lastUpdate[U"users"][i][U"vx"].get<float>();

						if(KeyW.pressed() == KeyS.pressed()) {
							lastUpdate[U"users"][i][U"vy"] = float(oVY - oVY * V_K);
						} else if(KeyW.pressed()) {
							lastUpdate[U"users"][i][U"vy"] =
									float(oVY + (-oVY / V_MAX - 1) * V_MAX * V_K);
						} else {
							lastUpdate[U"users"][i][U"vy"] =
									float(oVY + (1 - oVY / V_MAX) * V_MAX * V_K);
						}
						if(KeyA.pressed() == KeyD.pressed()) {
							lastUpdate[U"users"][i][U"vx"] = float(oVX - oVX * V_K);
						} else if(KeyA.pressed()) {
							lastUpdate[U"users"][i][U"vx"] =
									float(oVX + (-oVX / V_MAX - 1) * V_MAX * V_K);
						} else {
							lastUpdate[U"users"][i][U"vx"] =
									float(oVX + (1 - oVX / V_MAX) * V_MAX * V_K);
						}

						oY = lastUpdate[U"users"][i][U"y"].get<float>() - center.y +
								 (frame - lastUpdate[U"timestamp"].get<int>()) * oVY;
						oX = lastUpdate[U"users"][i][U"x"].get<float>() - center.x +
								 (frame - lastUpdate[U"timestamp"].get<int>()) * oVX;
					}
				}
			}

			// draw background
			int cy = MOD(int(-oY), resolution.y), cx = MOD(int(-oX), resolution.x),
					odd = MOD(int(-oY) / resolution.y, 2);

			// ballImage->draw(cx - resolution.x, cy - resolution.y * odd);
			// ballImage->draw(cx, cy - resolution.y * odd);
			// if(center.x < cx) {
			// 	cx -= center.x;
			// } else {
			// 	cx += center.x;
			// }
			// ballImage->draw(cx - resolution.x, cy - resolution.y * !odd);
			// ballImage->draw(cx, cy - resolution.y * !odd);

			backImage->draw(-oX, -oY);
			// backImage->overwrite(*backAndBallImage, 0, 0);

			// Circle{Cursor::Pos() / scaling, 20}.draw(ColorF{1, 1, 0, 0.5});

			if(lastUpdate.hasElement(U"users")) {
				auto users = lastUpdate[U"users"];
				auto bullets = lastUpdate[U"bullets"];
				auto feeds = lastUpdate[U"feeds"];

				// draw users

				for(const auto& user : users.arrayView()) {
					int uX = user[U"x"].get<float>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 user[U"vx"].get<float>();

					int uY = user[U"y"].get<float>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 user[U"vy"].get<float>();
					String uName = user[U"name"].get<String>();
					String uId = user[U"id"].get<String>();
					float uMass = user[U"mass"].get<float>();

					// draw ball
					int radius = std::powf(uMass, 1. / 3);
					Circle(uX - oX, uY - oY, radius + 1).draw(textColor1);
					Circle(uX - oX, uY - oY, radius).draw(ballColor);

					// draw name
					(*fontSmall)(uName).drawAt(Vec2(uX - oX, uY - oY - 5), textColor2);
				}

				for(const auto& e : bullets.arrayView()) {
				}
				for(const auto& e : feeds.arrayView()) {
				}
			}

			break;
		}
		case DEAD: {
			break;
		}
		default: {
			break;
		}
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
