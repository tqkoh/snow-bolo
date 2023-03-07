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

// WebSocket ws(API_URL);
std::unique_ptr<WebSocket> ws;
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
std::unique_ptr<Texture> miniMapImage;
std::unique_ptr<Audio> themeAudio;

JSON lastUpdate;
JSON lastMyUpdate;

JSON defaultUpdate;

int damage = 0;
int damageBarAnimationFrame = 0;

double oY = 0, oX = 0, oVY = 0, oVX = 0;
String id;

class damageAnimation {
public:
	String id;
	int y = 0, x = 0;
	int damage = 0;
	mutable int frame = 0;
	damageAnimation(int y, int x, int damage, int frame, String id)
			: y(y), x(x), damage(damage), frame(frame), id(id) {}
	int drawDamage(int oY, int oX) const {
		--frame;
		(*fontSmall)(damage).drawAt(x - oX, y - oY + 5, damageColor);
		return frame <= 0;
	}
};
const bool operator<(const damageAnimation& lhs, const damageAnimation& rhs) {
	if(lhs.id == rhs.id) {
		if(lhs.frame == rhs.frame) {
			return &lhs < &rhs;
		}
		return lhs.frame < rhs.frame;
	}

	return lhs.id < rhs.id;
}

std::set<damageAnimation> damageAnimations;

double radiusFromMass(double mass) {
	if(mass <= 0)
		return 1;
	double r6 = std::sqrt(6);
	if(mass > 2000. / 9. * r6) {
		return (-std::powf(Math::E, -(mass - 2000. / 9. * r6) / 10000) + 1) *
							 RADIUS_M +
					 10. / 3. * r6;
	}
	return std::pow(mass, 1. / 3.);
}

void load() {
	fontSmall =
			std::make_unique<Font>(FONT_SIZE_SMALL, FONT_PATH, FontStyle::Bitmap);
	fontMedium =
			std::make_unique<Font>(FONT_SIZE_MEDIUM, FONT_PATH, FontStyle::Bitmap);
	backImage = std::make_unique<Texture>(U"assets/images/back.png");
	ballImage = std::make_unique<Texture>(U"assets/images/ball.png");
	prepareImage = std::make_unique<Texture>(U"assets/images/prepare.png");
	katasaDekasaImage =
			std::make_unique<Texture>(U"assets/images/katasa_dekasa.png");
	miniMapImage = std::make_unique<Texture>(U"assets/images/minimap.png");

	themeAudio =
			std::make_unique<Audio>(U"assets/sounds/snowball_theme.mp3",
															Arg::loopBegin = 1741510, Arg::loopEnd = 2799910);
}

void init() {
	ws = std::make_unique<WebSocket>(API_URL);

	state = PREPARE;
	name.active = true;

	previnput[U"W"] = false;
	previnput[U"A"] = false;
	previnput[U"S"] = false;
	previnput[U"D"] = false;
	previnput[U"left"] = MouseL.pressed();
	previnput[U"right"] = MouseR.pressed();

	ClearPrint();

	defaultUpdate = JSON::Load(U"assets/data/defaultUpdate.json");

	themeAudio->play();
}
int update() {
	if(ws->disconnected) {
		Console << U"disconnected from server";
		Print << U"disconnected from server";
		return 1;
	}

	switch(state) {
		case PREPARE: {
			name.active = true;

			if(KeyEnter.down()) {
				JSON json, input;
				input[U"name"] = name.text;
				json[U"Method"] = U"join";
				json[U"Args"] = input;
				ws->SendText(json.formatUTF8Minimum());
				state = PLAYING;
				name.active = false;
				SimpleGUI::TextBox(name, Vec2(-1000, -1000), 100, 12, false);
			}

			break;
		}
		case PLAYING: {
			String received;
			while(ws->hasReceivedText()) {
				received = Unicode::FromUTF8(ws->getReceivedTextAndPopFromBuffer());
				// printf("received: %s\n", received.narrow().c_str());
				// try {
				if(received == U"") {
					lastUpdate = defaultUpdate;
					lastUpdate[U"timestamp"] = frame;
					continue;
				}
				JSON json = JSON::Parse(received, AllowExceptions::Yes);
				if(!json) {
					printf("parse failed: %s\n", received.narrow().c_str());
					lastUpdate = defaultUpdate;
					lastUpdate[U"timestamp"] = frame;
				}
				if(json[U"method"] == U"joinAccepted") {
					id = json[U"args"][U"id"].get<String>();
				}
				if(json[U"method"] == U"update") {
					lastUpdate = json[U"args"];
					lastUpdate[U"timestamp"] = frame;
				}
				// } catch(...) {
				// 	printf("parse failed: %s\n", received.narrow().c_str());
				// }
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
					ws->SendText(json.formatUTF8Minimum());
					previnput = input;

					// printf("time: %lld", std::time(nullptr));
				}
			}

			// update my data
			if(lastUpdate.hasElement(U"users")) {
				for(int i = 0; i < lastUpdate[U"users"].size(); i++) {
					double mass = lastUpdate[U"users"][i][U"mass"].get<double>();
					lastUpdate[U"users"][i][U"radius"] = radiusFromMass(mass);

					if(mass < 1) {
						printf("0\n");
					}

					if(lastUpdate[U"users"][i][U"id"].get<String>() == id) {
						if(mass < 1) {
							printf("0.1\n");
						}
						lastMyUpdate = lastUpdate[U"users"][i];

						oVY = lastUpdate[U"users"][i][U"vy"].get<double>();
						oVX = lastUpdate[U"users"][i][U"vx"].get<double>();
						if(mass < 1) {
							printf("0.2\n");
						}
						if(KeyW.pressed() == KeyS.pressed()) {
							lastUpdate[U"users"][i][U"vy"] = double(oVY - oVY * V_K);
						} else if(KeyW.pressed()) {
							lastUpdate[U"users"][i][U"vy"] =
									double(oVY + (-oVY / MAX_V - 1.) * MAX_V * V_K);
						} else {
							lastUpdate[U"users"][i][U"vy"] =
									double(oVY + (1. - oVY / MAX_V) * MAX_V * V_K);
						}
						if(KeyA.pressed() == KeyD.pressed()) {
							lastUpdate[U"users"][i][U"vx"] = double(oVX - oVX * V_K);
						} else if(KeyA.pressed()) {
							lastUpdate[U"users"][i][U"vx"] =
									double(oVX + (-oVX / MAX_V - 1.) * MAX_V * V_K);
						} else {
							lastUpdate[U"users"][i][U"vx"] =
									double(oVX + (1. - oVX / MAX_V) * MAX_V * V_K);
						}
						if(mass < 1) {
							printf("0.3\n");
						}
						oY = lastUpdate[U"users"][i][U"y"].get<double>() - center.y +
								 (frame - lastUpdate[U"timestamp"].get<int>()) * oVY;
						oX = lastUpdate[U"users"][i][U"x"].get<double>() - center.x +
								 (frame - lastUpdate[U"timestamp"].get<int>()) * oVX;
						if(mass < 1) {
							printf("0.4\n");
						}
					}
				}
			}

			if(lastUpdate.hasElement(U"mass") &&
				 lastMyUpdate[U"mass"].get<double>() < 1) {
				printf("1\n");
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

			if(lastUpdate.hasElement(U"mass") &&
				 lastMyUpdate[U"mass"].get<double>() < 1) {
				printf("2\n");
			}

			if(lastUpdate.hasElement(U"users")) {
				auto users = lastUpdate[U"users"];
				auto bullets = lastUpdate[U"bullets"];
				auto feeds = lastUpdate[U"feeds"];

				// draw users

				auto uArray = users.arrayView();
				for(int i = 0; i < users.size(); ++i) {
					auto&& user = uArray[i];
					int uY = user[U"y"].get<double>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 user[U"vy"].get<double>();
					int uX = user[U"x"].get<double>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 user[U"vx"].get<double>();
					if(user[U"id"].get<String>() == id) {
						uY = oY + center.y;
						uX = oX + center.x;
					}
					String uName = user[U"name"].get<String>();
					String uId = user[U"id"].get<String>();
					double uMass = user[U"mass"].get<double>();
					int uDy = user[U"dy"].get<int>();
					int uDx = user[U"dx"].get<int>();
					int uLeftClickLength = user[U"leftClickLength"].get<int>();
					int uDamage = user[U"damage"].get<int>();

					// draw ball
					int radius = user[U"radius"].get<double>();
					Circle(uX - oX, uY - oY, radius)
							.draw(ballColor)
							.drawFrame(0, 1, textColor1);
					if(user[U"damage"].get<int>() < 0) {
						Triangle(uX - oX, uY - oY + radius + 5, 5, 0).draw(recoverColor);
						Triangle(uX - oX - radius - 5, uY - oY, 5, Math::Pi * 1 / 2)
								.draw(recoverColor);
						Triangle(uX - oX, uY - oY - radius - 5, 5, Math::Pi * 2 / 2)
								.draw(recoverColor);
						Triangle(uX - oX + radius + 5, uY - oY, 5, Math::Pi * 3 / 2)
								.draw(recoverColor);
					}

					// draw triangle looking at users cursor
					if(uLeftClickLength) {
						if(uId == id) {
							uDy = Cursor::Pos().y / scaling - center.y;
							uDx = Cursor::Pos().x / scaling - center.x;
						}

						radius += 5 + uLeftClickLength / 20;
						double l = std::sqrt(uDx * uDx + uDy * uDy);
						if(!l) {
							l = nextafter(0, 1);
						}
						double dy = uDy / l, dx = uDx / l;
						if(!dx)
							dx = nextafter(dx, DBL_MAX);
						double y = uY - oY + dy * radius, x = uX - oX + dx * radius;
						Triangle(x, y, 5 + uLeftClickLength / 10,
										 std::atan(dy / dx) + (dx < 0) * Math::Pi + Math::Pi / 2.)
								.draw(textColor2);
					}

					// draw name
					(*fontSmall)(uName).drawAt(Vec2(uX - oX, uY - oY), textColor2);

					if(uDamage > 0 && uId != id) {
						damageAnimations.insert(
								damageAnimation(uY, uX, user[U"damage"].get<int>(),
																ANIMATION_DAMAGE_LENGTH, uId));
						user[U"damage"] = 0;
					}
				}
				for(const auto& e : bullets.arrayView()) {
					int bY = e[U"y"].get<double>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 e[U"vy"].get<double>();
					int bX = e[U"x"].get<double>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 e[U"vx"].get<double>();
					int radius = radiusFromMass(e[U"mass"].get<double>());

					Circle(bX - oX, bY - oY, radius)
							.draw(ballColor)
							.drawFrame(0, 1, textColor2);
				}
				for(const auto& e : feeds.arrayView()) {
					int fY = e[U"y"].get<double>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 e[U"vy"].get<double>();
					int fX = e[U"x"].get<double>() +
									 (frame - lastUpdate[U"timestamp"].get<int>()) *
											 e[U"vx"].get<double>();
					int radius = radiusFromMass(e[U"mass"].get<double>());

					Circle(fX - oX, fY - oY, radius)
							.draw(ballColor)
							.drawFrame(0, 1, paintColor1);
				}

				// draw damage
				for(auto it = damageAnimations.begin(); it != damageAnimations.end();) {
					auto next = it;
					++next;
					if(next != damageAnimations.end() && it->id == next->id) {
						it = damageAnimations.erase(it);
					} else if(it->drawDamage(oY, oX)) {
						it = damageAnimations.erase(it);
					} else {
						++it;
					}
				}
			}

			if(lastUpdate.hasElement(U"mass") &&
				 lastMyUpdate[U"mass"].get<double>() < 1) {
				printf("3\n");
			}

			if(lastMyUpdate.hasElement(U"damage")) {
				katasaDekasaImage->draw(0, resolution.y - 50);
				const int strength = lastMyUpdate[U"strength"].get<int>();
				const double mass = lastMyUpdate[U"mass"].get<double>();
				const double radius = lastMyUpdate[U"radius"].get<double>();
				const int lastDamage = lastMyUpdate[U"damage"].get<int>();
				damage = lastDamage > 0 ? lastDamage : damage;
				damageBarAnimationFrame += lastDamage;
				const double katasa_w = strength * 270 / 100;
				const double damage_w = damageBarAnimationFrame * 270 / 100;
				const double dekasa_w = radius / (RADIUS_M + 10. / 3. * sqrt(6)) * 270;

				damageBarAnimationFrame /= 1.5;

				Rect(GAME_KATASA_DEKASA_X + katasa_w, GAME_KATASA_Y, damage_w,
						 GAME_KATASA_DEKASA_HEIGHT)
						.draw(damageBarAnimationFrame > 0 ? damageColor : recoverColor)
						.drawFrame(
								0, 1, damageBarAnimationFrame > 0 ? damageColor : recoverColor);
				Rect(GAME_KATASA_DEKASA_X, GAME_KATASA_Y, katasa_w,
						 GAME_KATASA_DEKASA_HEIGHT)
						.draw(paintColor1)
						.drawFrame(0, 1, textColor1);
				(*fontSmall)(strength).draw(GAME_KATASA_DEKASA_X + 2 + 1, GAME_KATASA_Y,
																		shadowColor);
				(*fontSmall)(strength).draw(GAME_KATASA_DEKASA_X + 2, GAME_KATASA_Y,
																		textColor1);
				Rect(GAME_KATASA_DEKASA_X, GAME_DEKASA_Y, dekasa_w,
						 GAME_KATASA_DEKASA_HEIGHT)
						.draw(paintColor2)
						.drawFrame(0, 1, textColor2);
				(*fontSmall)(int(mass)).draw(GAME_KATASA_DEKASA_X + 2 + 1,
																		 GAME_DEKASA_Y, shadowColor);
				(*fontSmall)(int(mass)).draw(GAME_KATASA_DEKASA_X + 2, GAME_DEKASA_Y,
																		 textColor2);

				miniMapImage->draw(resolution - Vec2(50, 50));
				if(lastMyUpdate.hasElement(U"x")) {
					Circle(resolution - Vec2(50, 50) +
										 Vec2(lastMyUpdate[U"x"].get<double>() / 36,
													lastMyUpdate[U"y"].get<double>() / 36),
								 lastMyUpdate[U"radius"].get<double>() / 36 + 1)
							.draw(textColor1);
				}
			}

			if(lastUpdate.hasElement(U"mass") &&
				 lastMyUpdate[U"mass"].get<double>() < 1) {
				printf("4\n");
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
