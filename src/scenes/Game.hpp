#pragma once

#include <deque>
#include "../Const.hpp"
#include "../Global.hpp"
#include "lib/WebSocket.hpp"

#define MOD(n, m) (((n) % (m) + (m)) % (m))

EM_JS(void, openLink, (const char* url), {window.open(UTF8ToString(url))});

namespace Game {

std::unique_ptr<WebSocket> ws;

enum GameState {
	PREPARE,
	PLAYING,

	GameState_NUM
} state = PREPARE;

int debugLevel = 0;

// WebSocket ws(API_URL);
TextEditState name;
Point center(resolution / 2);

std::unique_ptr<Font> fontSmall;
std::unique_ptr<Font> fontMedium;
std::unique_ptr<Font> fontLarge;
JSON previnput;

std::unique_ptr<Texture> backImage;
std::unique_ptr<Texture> ballImage;
std::unique_ptr<Texture> prepareImage;
std::unique_ptr<Texture> katasaDekasaImage;
std::unique_ptr<Texture> miniMapImage;
std::unique_ptr<Texture> resultImage;
std::unique_ptr<Audio> themeAudio;

JSON lastUpdate;
JSON lastMyUpdate;

JSON defaultUpdate;

int damage = 0;
double damageBarAnimation = 0;

double oY = 0, oX = 0, oVY = 0, oVX = 0;
String id;
String myId;

enum SpectateMode {
	OFF,
	PLAYER,
	MAP,

	SpectateMode_NUM
} spectateMode = OFF;

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

struct chatMessage {
	String message;
	int timestamp;
	chatMessage(String message, int timestamp)
			: message(message), timestamp(timestamp) {}
};

std::deque<chatMessage> chatMessages;

String shorten(String str, int l) {
	if(str.size() > l) {
		return str.substr(0, l - 2) + U"..";
	}
	return String(l - str.size(), U' ') + str;
}
struct leaderBoardRow {
	String id;
	String name;
	String massString;
	leaderBoardRow(String id, String name, int mass) : id(id) {
		this->name = shorten(name, 8);
		if(mass < 1000)
			massString = ToString(mass);
		else if(mass < 1000000)
			massString = ToString(mass / 1000) + U"K";
		else {
			massString = ToString(mass / 1000000) + U"M";
		}
	}
};

std::vector<leaderBoardRow> leaderBoard;

bool resultShowing = false;
int maxMass = 0;
int bestRank = 998244353;

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

std::map<std::string, bool> keys;

struct HistoryRecord {
	int frame, mass, strength;
	HistoryRecord(int frame, int mass, int strength)
			: frame(frame), mass(mass), strength(strength) {}
};
std::vector<HistoryRecord> history;
LineString historyMassLine;
std::vector<Vec2> historyStrength;
Polygon historyStrengthPolygon;
int64_t joinedFrame = 0;
int64_t deadFrame = 998244353;

void load() {
	fontSmall =
			std::make_unique<Font>(FONT_SIZE_SMALL, FONT_PATH, FontStyle::Bitmap);
	fontMedium =
			std::make_unique<Font>(FONT_SIZE_MEDIUM, FONT_PATH, FontStyle::Bitmap);
	fontLarge =
			std::make_unique<Font>(FONT_SIZE_LARGE, FONT_PATH, FontStyle::Bitmap);
	backImage = std::make_unique<Texture>(U"assets/images/back.png");
	ballImage = std::make_unique<Texture>(U"assets/images/ball.png");
	prepareImage = std::make_unique<Texture>(U"assets/images/prepare.png");
	katasaDekasaImage =
			std::make_unique<Texture>(U"assets/images/katasa_dekasa.png");
	miniMapImage = std::make_unique<Texture>(U"assets/images/minimap.png");
	resultImage = std::make_unique<Texture>(U"assets/images/result.png");

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

	Print.setFont(*fontLarge);
	ClearPrint();

	defaultUpdate = JSON::Load(U"assets/data/defaultUpdate.json");

	themeAudio->play();
}
void join() {
	JSON json, input;
	input[U"name"] = name.text;
	json[U"method"] = U"join";
	json[U"Args"] = input;
	ws->SendText(json.formatUTF8Minimum());
	state = PLAYING;
	name.active = false;
	spectateMode = OFF;
	resultShowing = false;
	history.clear();
	history.emplace_back(frame, 1, 100);
	joinedFrame = frame;
	deadFrame = 998244353;
	maxMass = 0;
	bestRank = 998244353;
}
int update() {
	if(ws->disconnected) {
		Console << U"disconnected from server";
		Print << U"disconnected from server";
		return 1;
	}

	{
		keys["W"] = KeyW.pressed() || KeyUp.pressed();
		keys["A"] = KeyA.pressed() || KeyLeft.pressed();
		keys["S"] = KeyS.pressed() || KeyDown.pressed();
		keys["D"] = KeyD.pressed() || KeyRight.pressed();
	}

	switch(state) {
		case PREPARE: {
			name.active = true;

			if(KeyEnter.down()) {
				join();
				SimpleGUI::TextBox(name, Vec2(-1000, -1000), 100, 12, false);
			}

			break;
		}
		case PLAYING: {
			if(debugLevel > 5)
				printf("update.0");
			String received;
			// process received data
			{
				int dead = 0;
				while(ws->hasReceivedText()) {
					received = Unicode::FromUTF8(ws->getReceivedTextAndPopFromBuffer());
					// 0&&printf("received: %s\n", received.narrow().c_str());
					// try {
					if(received == U"") {
						lastUpdate = defaultUpdate;
						lastUpdate[U"timestamp"] = frame;
						continue;
					}
					JSON json = JSON::Parse(received, AllowExceptions::Yes);
					if(!json) {
						0 && printf("parse failed: %s\n", received.narrow().c_str());
						lastUpdate = defaultUpdate;
						lastUpdate[U"timestamp"] = frame;
					}
					if(json[U"method"] == U"joinAccepted") {
						myId = json[U"args"][U"id"].get<String>();
						id = myId;
						spectateMode = OFF;
					} else if(json[U"method"] == U"dead") {
						dead = lastMyUpdate[U"strength"].get<int>();
						resultShowing = true;
						spectateMode = MAP;
						deadFrame = frame;

						int aliveLength = deadFrame - joinedFrame;

						historyMassLine.clear();
						historyStrength.clear();
						historyStrength.emplace_back(HISTORY_X, HISTORY_Y + HISTORY_H);
						for(auto e : history) {
							Vec2 massPoint(
									HISTORY_X + (e.frame - joinedFrame) * HISTORY_W / aliveLength,
									HISTORY_Y +
											(1. - double(e.mass) / double(maxMass)) * HISTORY_H);
							historyMassLine.emplace_back(massPoint);

							Vec2 strengthPoint(
									HISTORY_X + (e.frame - joinedFrame) * HISTORY_W / aliveLength,
									HISTORY_Y + HISTORY_H -
											(double(e.strength) / 100.) *
													(double(e.mass) / double(maxMass) * HISTORY_H));
							historyStrength.emplace_back(strengthPoint);
						}
						historyStrength.emplace_back(HISTORY_X + HISTORY_W,
																				 HISTORY_Y + HISTORY_H);
						historyStrengthPolygon = Polygon(historyStrength);
					} else if(json[U"method"] == U"update") {
						lastUpdate = json[U"args"];
						lastUpdate[U"timestamp"] = frame;
					} else if(json[U"method"] == U"message") {
						String message = json[U"args"][U"message"].get<String>();
						chatMessages.emplace_back(message, frame);
					}
					// } catch(...) {
					// 	0&&printf("parse failed: %s\n", received.narrow().c_str());
					// }
				}
				if(dead) {
					lastMyUpdate[U"damage"] = dead;
					lastMyUpdate[U"strength"] = 0;
				}
			}

			if(debugLevel > 5)
				printf("update.1");

			if(spectateMode == OFF) {
				// send input
				JSON json, input;
				auto p = Cursor::Pos() / scaling;
				input[U"W"] = keys["W"];
				input[U"A"] = keys["A"];
				input[U"S"] = keys["S"];
				input[U"D"] = keys["D"];
				input[U"left"] = MouseL.pressed();
				input[U"right"] = MouseR.pressed();
				input[U"dy"] = p.y - center.y;
				input[U"dx"] = p.x - center.x;

				json[U"method"] = U"input";
				json[U"args"] = input;

				// look at the md #2
				if(MouseL.pressed() && frame % SEND_INPUT_PER == 0 ||
					 MouseR.pressed() && frame % SEND_INPUT_PER == 0 ||
					 keys["W"] != previnput[U"W"].get<bool>() ||
					 keys["A"] != previnput[U"A"].get<bool>() ||
					 keys["S"] != previnput[U"S"].get<bool>() ||
					 keys["D"] != previnput[U"D"].get<bool>() ||
					 MouseL.pressed() != previnput[U"left"].get<bool>() ||
					 MouseR.pressed() != previnput[U"right"].get<bool>()) {
					ws->SendText(json.formatUTF8Minimum());
					previnput = input;

					// 0&&printf("time: %lld", std::time(nullptr));
				}

				// update history
				if(lastMyUpdate.hasElement(U"strength") &&
					 history.back().frame + HISTORY_MIN_INTERVAL < frame &&
					 HISTORY_W * (frame - history.back().frame) / (frame - joinedFrame) >=
							 HISTORY_MIN_APPEND_W) {
					// printf("emplace_back");
					// printf("%lld %d %d", frame, lastMyUpdate[U"strength"].get<int>(),
					// 			 lastMyUpdate[U"damage"].get<int>());
					history.emplace_back(
							HistoryRecord(frame, int(lastMyUpdate[U"mass"].get<double>()),
														lastMyUpdate[U"strength"].get<int>()));
				}
			} else {
				if(spectateMode == PLAYER) {
					if(KeyShift.down()) {
						spectateMode = MAP;
						id = U"";
					}
				}
				if(resultShowing && KeyEnter.down()) {
					join();
				}

				if(resultShowing && KeyT.down()) {
					String url = TWEET_URL(maxMass);
					openLink(url.narrow().c_str());
				}
				if(KeyEscape.down()) {
					resultShowing = !resultShowing;
				}
			}

			if(debugLevel > 5)
				printf("update.2");

			// update users' data
			leaderBoard.clear();
			if(lastUpdate.hasElement(U"users")) {
				for(int i = 0; i < lastUpdate[U"users"].size(); i++) {
					double mass = lastUpdate[U"users"][i][U"mass"].get<double>();
					lastUpdate[U"users"][i][U"radius"] = radiusFromMass(mass);

					if(i < 5) {
						leaderBoard.emplace_back(
								lastUpdate[U"users"][i][U"id"].get<String>(),
								lastUpdate[U"users"][i][U"name"].get<String>(),
								int(lastUpdate[U"users"][i][U"mass"].get<double>()));
					}

					if(mass < 1) {
						0 && printf("0\n");
					}

					if(lastUpdate[U"users"][i][U"id"].get<String>() == id) {
						maxMass = std::max(maxMass, int(mass));
						bestRank = std::min(bestRank, i + 1);
						if(mass < 1) {
							0 && printf("0.1\n");
						}
						lastMyUpdate = lastUpdate[U"users"][i];

						// conceal lag
						oVY = lastUpdate[U"users"][i][U"vy"].get<double>();
						oVX = lastUpdate[U"users"][i][U"vx"].get<double>();
						if(mass < 1) {
							0 && printf("0.2\n");
						}
						oY = lastUpdate[U"users"][i][U"y"].get<double>() - center.y +
								 (frame - lastUpdate[U"timestamp"].get<int>()) * oVY;
						oX = lastUpdate[U"users"][i][U"x"].get<double>() - center.x +
								 (frame - lastUpdate[U"timestamp"].get<int>()) * oVX;
					}
				}
			}

			if(debugLevel > 5)
				printf("update.3");

			if(spectateMode == MAP) {
				0 && printf("00");
				// update origin using input
				if(keys["W"] == keys["S"]) {
					oVY = double(oVY - oVY * V_K);
				} else if(keys["W"]) {
					oVY = double(oVY + (-oVY / MAX_V - 1.) * MAX_V * V_K);
				} else {
					oVY = double(oVY + (1. - oVY / MAX_V) * MAX_V * V_K);
				}
				if(keys["A"] == keys["D"]) {
					oVX = double(oVX - oVX * V_K);
				} else if(keys["A"]) {
					oVX = double(oVX + (-oVX / MAX_V - 1.) * MAX_V * V_K);
				} else {
					oVX = double(oVX + (1. - oVX / MAX_V) * MAX_V * V_K);
				}
				oY += oVY;
				oX += oVX;
			}
			if(spectateMode == MAP || spectateMode == PLAYER) {
				// start spectating user clicked
				if(MouseL.down()) {
					Vec2 p = Cursor::Pos() / scaling + Vec2(oX, oY);
					int startedSpectating = 0;
					for(int i = 0; i < lastUpdate[U"users"].size(); i++) {
						double radius = lastUpdate[U"users"][i][U"radius"].get<double>();
						Vec2 pos(lastUpdate[U"users"][i][U"x"].get<double>(),
										 lastUpdate[U"users"][i][U"y"].get<double>());
						if((p - pos).length() < radius) {
							spectateMode = PLAYER;
							id = lastUpdate[U"users"][i][U"id"].get<String>();
							startedSpectating = 1;
							chatMessages.emplace_back(
									U"spectating {}"_fmt(
											lastUpdate[U"users"][i][U"name"].get<String>()),
									frame);
							chatMessages.emplace_back(U"shift to dismount", frame);

							break;
						}
					}
					if(!startedSpectating) {
						spectateMode = MAP;
						id = U"";
					}
				}
			}
			if(debugLevel > 5)
				printf("update.1");
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
			if(spectateMode == MAP)
				0 && printf("33\n");
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
			Rect(MAP_MARGIN - oX, MAP_MARGIN - oY, MAP_WIDTH - MAP_MARGIN * 2,
					 MAP_HEIGHT - MAP_MARGIN * 2)
					.drawFrame(0, 1, shadowColor);
			// backImage->overwrite(*backAndBallImage, 0, 0);

			// Circle{Cursor::Pos() / scaling, 20}.draw(ColorF{1, 1, 0, 0.5});

			if(lastUpdate.hasElement(U"mass") &&
				 lastMyUpdate[U"mass"].get<double>() < 1) {
				if(spectateMode == MAP)
					0 && printf("2\n");
			}

			if(lastUpdate.hasElement(U"users")) {
				auto users = lastUpdate[U"users"];
				auto bullets = lastUpdate[U"bullets"];
				auto feeds = lastUpdate[U"feeds"];

				if(spectateMode == MAP)
					0 && printf("44\n");
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

					if(spectateMode == MAP)
						0 && printf("44.0.1\n");
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

					if(spectateMode == MAP)
						0 && printf("44.0.2\n");
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

					if(spectateMode == MAP)
						0 && printf("44.0.3\n");
					// draw name
					(*fontSmall)(uName).drawAt(Vec2(uX - oX, uY - oY), textColor2);

					if(uDamage > 0 && uId != id) {
						damageAnimations.insert(
								damageAnimation(uY, uX, user[U"damage"].get<int>(),
																ANIMATION_DAMAGE_LENGTH, uId));
						user[U"damage"] = 0;
					}
				}

				if(spectateMode == MAP)
					0 && printf("44.1\n");

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
				if(spectateMode == MAP)
					0 && printf("44.2\n");
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

				if(spectateMode == MAP)
					0 && printf("55\n");
			}

			if(lastMyUpdate.hasElement(U"damage")) {
				katasaDekasaImage->draw(0, resolution.y - 50);
				const int strength = lastMyUpdate[U"strength"].get<int>();
				const double mass = lastMyUpdate[U"mass"].get<double>();
				const double radius = lastMyUpdate[U"radius"].get<double>();
				const int lastDamage = lastMyUpdate[U"damage"].get<int>();
				if(lastDamage > 0) {
					damage = lastDamage;
				}
				damageBarAnimation /= 1.5;
				damageBarAnimation += lastDamage;
				lastMyUpdate[U"damage"] = 0;
				const int katasa_w = strength * 270 / 100;
				const int damage_w = std::max(0., damageBarAnimation * 270 / 100 - 2);
				const int dekasa_w = radius / (RADIUS_M + 10. / 3. * sqrt(6)) * 270;

				Rect(GAME_KATASA_DEKASA_X, GAME_KATASA_Y, katasa_w,
						 GAME_KATASA_DEKASA_HEIGHT)
						.draw(paintColor1)
						.drawFrame(0, 1, textColor1);
				Rect(GAME_KATASA_DEKASA_X + katasa_w + 2, GAME_KATASA_Y, damage_w,
						 GAME_KATASA_DEKASA_HEIGHT)
						.draw(damageBarAnimation > 0 ? damageColor : recoverColor)
						.drawFrame(0, 1,
											 damageBarAnimation > 0 ? damageColor : recoverColor);

				{
					String strengthString = U"{}"_fmt(strength);
					if(spectateMode == PLAYER || spectateMode == MAP) {
						strengthString =
								U"{}: {}"_fmt(lastMyUpdate[U"name"].get<String>(), strength);
					}
					(*fontSmall)(strengthString)
							.draw(GAME_KATASA_DEKASA_X + 2 + 1, GAME_KATASA_Y, shadowColor);
					(*fontSmall)(strengthString)
							.draw(GAME_KATASA_DEKASA_X + 2, GAME_KATASA_Y, textColor1);
				}
				Rect(GAME_KATASA_DEKASA_X, GAME_DEKASA_Y, dekasa_w,
						 GAME_KATASA_DEKASA_HEIGHT)
						.draw(paintColor2)
						.drawFrame(0, 1, textColor2);
				{
					String massString = U"{}"_fmt(int(mass));
					if(spectateMode == PLAYER || spectateMode == MAP) {
						massString =
								U"{}: {}"_fmt(lastMyUpdate[U"name"].get<String>(), int(mass));
					}
					(*fontSmall)(massString)
							.draw(GAME_KATASA_DEKASA_X + 2 + 1, GAME_DEKASA_Y, shadowColor);
					(*fontSmall)(massString)
							.draw(GAME_KATASA_DEKASA_X + 2, GAME_DEKASA_Y, textColor2);
				}

				miniMapImage->draw(resolution - Vec2(50, 50));
				if(spectateMode == MAP) {
					Rect(resolution.x - 50 + oX / 36, resolution.y - 50 + oY / 36,
							 resolution.x / 36, resolution.y / 36)
							.drawFrame(0, 1, textColor1);
				} else if(lastMyUpdate.hasElement(U"x")) {
					Circle(resolution - Vec2(50, 50) +
										 Vec2(lastMyUpdate[U"x"].get<double>() / 36,
													lastMyUpdate[U"y"].get<double>() / 36),
								 lastMyUpdate[U"radius"].get<double>() / 36 + 1)
							.draw(textColor1);
				}
			}

			{
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

			if(spectateMode == MAP)
				0 && printf("66\n");

			{
				// draw leaderboard

				(*fontSmall)(U"Leaderboard").draw(LEADERBOARD_X + 1, 10, shadowColor);
				(*fontSmall)(U"Leaderboard").draw(LEADERBOARD_X, 10, textColor2);

				for(int i = 0; i < leaderBoard.size(); ++i) {
					auto e = leaderBoard[i];
					if(e.id == myId) {
						(*fontSmall)(U"{}. {}: {}"_fmt(i + 1, e.name, e.massString))
								.draw(LEADERBOARD_X + 1, 30 + i * 20, shadowColor);
						(*fontSmall)(U"{}. {}: {}"_fmt(i + 1, e.name, e.massString))
								.draw(LEADERBOARD_X, 30 + i * 20, textColor2);
					} else {
						(*fontSmall)(U"{}. {}: {}"_fmt(i + 1, e.name, e.massString))
								.draw(LEADERBOARD_X + 1, 30 + i * 20, shadowColor);
						(*fontSmall)(U"{}. {}: {}"_fmt(i + 1, e.name, e.massString))
								.draw(LEADERBOARD_X, 30 + i * 20, textColor1);
					}
				}
			}

			{
				// draw chat
				while(chatMessages.size()) {
					auto&& e = chatMessages.front();
					if(e.timestamp + CHAT_REMAIN < frame) {
						chatMessages.pop_front();
					} else {
						break;
					}
				}
				while(chatMessages.size() > 10) {
					chatMessages.pop_front();
				}
				{
					int i = 0, n = chatMessages.size();
					for(auto e : chatMessages) {
						++i;
						(*fontSmall)(e.message).draw(
								11, resolution.y - 90 - (n - 1 - i) * 20, shadowColor);
						(*fontSmall)(e.message).draw(
								10, resolution.y - 90 - (n - 1 - i) * 20, textColor1);
					}
				}
			}

			{
				if(resultShowing) {
					// draw result
					resultImage->draw(0, 0);
					Rect(HISTORY_X, HISTORY_Y, HISTORY_W, HISTORY_H)
							.drawFrame(0, 1, shadowColor);

					historyStrengthPolygon.draw(paintColor1);
					historyMassLine.draw(2, textColor2);

					(*fontSmall)(shorten(U"{}"_fmt(maxMass), 8))
							.draw(90 + 1, 84, shadowColor);
					(*fontSmall)(shorten(U"{}"_fmt(maxMass), 8)).draw(90, 84, textColor2);
					(*fontSmall)(shorten(U"{}"_fmt(bestRank), 8))
							.draw(90 + 1, 119, shadowColor);
					(*fontSmall)(shorten(U"{}"_fmt(bestRank), 8))
							.draw(90, 119, textColor2);
				}
			}
			break;
		}
		default: {
			break;
		}
	}
}

}	 // namespace Game
