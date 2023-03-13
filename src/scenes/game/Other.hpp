#pragma once

#include <Siv3D.hpp>

namespace Game {

std::unique_ptr<WebSocket> ws;

bool windowVisible = true;

enum GameState {
	PREPARE,
	PLAYING,

	GameState_NUM
} state = PREPARE;

int debugLevel = 0;

// WebSocket ws(API_URL);
TextEditState name;
Point center(resolution / 2);

JSON previnput;
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

int64_t joinedFrame = 0;
int64_t deadFrame = 998244353;
int kills = 0;

}	 // namespace Game
