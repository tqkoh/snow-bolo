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
std::unique_ptr<Texture> katasaDekasaImage;

JSON lastUpdate;
JSON lastMyUpdate;

double oY = 0, oX = 0, oVY = 0, oVX = 0;
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
	katasaDekasaImage =
			std::make_unique<Texture>(U"assets/images/katasa_dekasa.png");

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
				auto p = Cursor::Pos() / scaling;
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

			// update my data
			if(lastUpdate.hasElement(U"users")) {
				for(int i = 0; i < lastUpdate[U"users"].size(); i++) {
					if(lastUpdate[U"users"][i][U"id"].get<String>() == id) {
						lastMyUpdate = lastUpdate[U"users"][i];

						oVY = lastUpdate[U"users"][i][U"vy"].get<double>();
						oVX = lastUpdate[U"users"][i][U"vx"].get<double>();

						if(KeyW.pressed() == KeyS.pressed()) {
							lastUpdate[U"users"][i][U"vy"] = double(oVY - oVY * V_K);
						} else if(KeyW.pressed()) {
							lastUpdate[U"users"][i][U"vy"] =
									double(oVY + (-oVY / V_MAX - 1.) * V_MAX * V_K);
						} else {
							lastUpdate[U"users"][i][U"vy"] =
									double(oVY + (1. - oVY / V_MAX) * V_MAX * V_K);
						}
						if(KeyA.pressed() == KeyD.pressed()) {
							lastUpdate[U"users"][i][U"vx"] = double(oVX - oVX * V_K);
						} else if(KeyA.pressed()) {
							lastUpdate[U"users"][i][U"vx"] =
									double(oVX + (-oVX / V_MAX - 1.) * V_MAX * V_K);
						} else {
							lastUpdate[U"users"][i][U"vx"] =
									double(oVX + (1. - oVX / V_MAX) * V_MAX * V_K);
						}

						oY = lastUpdate[U"users"][i][U"y"].get<double>() - center.y +
								 (frame - lastUpdate[U"timestamp"].get<int>()) * oVY;
						oX = lastUpdate[U"users"][i][U"x"].get<double>() - center.x +
								 (frame - lastUpdate[U"timestamp"].get<int>()) * oVX;
					}
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

			katasaDekasaImage->draw(0, resolution.y - 50);
			Rect(GAME_KATASA_DEKASA_X, GAME_KATASA_Y, 35, GAME_KATASA_DEKASA_HEIGHT)
					.draw(paintColor1)
					.drawFrame(0, 1, textColor1);
			Rect(GAME_KATASA_DEKASA_X, GAME_DEKASA_Y, 17, GAME_KATASA_DEKASA_HEIGHT)
					.draw(paintColor2)
					.drawFrame(0, 1, textColor2);
			break;
		}
		case PLAYING: {
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
					int uY = user[U"y"].get<double>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 user[U"vy"].get<double>();
					int uX = user[U"x"].get<double>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 user[U"vx"].get<double>();
					String uName = user[U"name"].get<String>();
					String uId = user[U"id"].get<String>();
					double uMass = user[U"mass"].get<double>();
					int uDy = user[U"dy"].get<int>();
					int uDx = user[U"dx"].get<int>();
					int uLeftClickLength = user[U"leftClickLength"].get<int>();

					// draw ball
					int radius = std::powf(uMass, 1. / 3);
					Circle(uX - oX, uY - oY, radius)
							.draw(ballColor)
							.drawFrame(0, 1, textColor1);

					// draw triangle looking at users cursor
					if(uLeftClickLength) {
						radius += 5 + uLeftClickLength / 20;
						double l = std::sqrt(uDx * uDx + uDy * uDy);
						if(l != 0) {
							double dx = uDx / l, dy = uDy / l;
							if(!dx)
								dx = nextafter(dx, DBL_MAX);
							double x = uX - oX + dx * radius, y = uY - oY + dy * radius;
							Triangle(x, y, 5 + uLeftClickLength / 10,
											 std::atan(dy / dx) + (dx < 0) * Math::Pi + Math::Pi / 2.)
									.draw(textColor2);
						}
					}

					// draw name
					(*fontSmall)(uName).drawAt(Vec2(uX - oX, uY - oY - 5), textColor2);
				}

				for(const auto& e : bullets.arrayView()) {
				}
				for(const auto& e : feeds.arrayView()) {
				}
			}

			if(lastMyUpdate.hasElement(U"strength")) {
				katasaDekasaImage->draw(0, resolution.y - 50);
				const int strength = lastMyUpdate[U"strength"].get<int>();
				const double mass = lastMyUpdate[U"mass"].get<double>();
				const int dekasa_w = mass / 1000 + 1;
				Rect(GAME_KATASA_DEKASA_X, GAME_KATASA_Y, strength,
						 GAME_KATASA_DEKASA_HEIGHT)
						.draw(paintColor1)
						.drawFrame(0, 1, textColor1);
				(*fontSmall)(strength).draw(GAME_KATASA_DEKASA_X + 2, GAME_KATASA_Y,
																		textColor1);
				Rect(GAME_KATASA_DEKASA_X, GAME_DEKASA_Y, dekasa_w,
						 GAME_KATASA_DEKASA_HEIGHT)
						.draw(paintColor2)
						.drawFrame(0, 1, textColor2);
				(*fontSmall)(int(mass)).draw(GAME_KATASA_DEKASA_X + 2, GAME_DEKASA_Y,
																		 textColor2);
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
