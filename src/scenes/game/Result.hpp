#pragma once

#include <Siv3D.hpp>

#include "Other.hpp"

namespace Game {

enum SpectateMode {
	OFF,
	PLAYER,
	MAP,

	SpectateMode_NUM
} spectateMode = OFF;

bool resultShowing = false;
int maxMass = 0;
int bestRank = 998244353;

JSON lastUpdate;
JSON lastMyUpdate;
JSON defaultUpdate;

class History {
public:
	struct HistoryRecord {
		int frame, mass, strength;
		HistoryRecord(int frame, int mass, int strength)
				: frame(frame), mass(mass), strength(strength) {}
	};
	std::vector<HistoryRecord> historyRecords;
	LineString historyMassLine;
	std::vector<Vec2> historyStrength;
	Polygon historyStrengthPolygon;

	History() {}
	void init() {
		historyRecords.clear();
		historyRecords.emplace_back(frame, 1, 100);
	}
	void update() {
		if(lastMyUpdate.hasElement(U"strength") &&
			 historyRecords.back().frame + HISTORY_MIN_INTERVAL < frame &&
			 HISTORY_W * (frame - historyRecords.back().frame) /
							 (frame - joinedFrame) >=
					 HISTORY_MIN_APPEND_W) {
			// printf("emplace_back");
			// printf("%lld %d %d", frame,
			// lastMyUpdate[U"strength"].get<int>(),
			// 			 lastMyUpdate[U"damage"].get<int>());
			historyRecords.emplace_back(
					HistoryRecord(frame, int(lastMyUpdate[U"mass"].get<double>()),
												lastMyUpdate[U"strength"].get<int>()));
		}
	}
	void prepareDraw() {
		int aliveLength = deadFrame - joinedFrame;

		historyMassLine.clear();
		historyStrength.clear();
		historyStrength.emplace_back(HISTORY_X, HISTORY_Y + HISTORY_H);
		for(auto e : historyRecords) {
			Vec2 massPoint(
					HISTORY_X + (e.frame - joinedFrame) * HISTORY_W / aliveLength,
					HISTORY_Y + (1. - double(e.mass) / double(maxMass)) * HISTORY_H);
			historyMassLine.emplace_back(massPoint);

			Vec2 strengthPoint(
					HISTORY_X + (e.frame - joinedFrame) * HISTORY_W / aliveLength,
					HISTORY_Y + HISTORY_H -
							(double(e.strength) / 100.) *
									(double(e.mass) / double(maxMass) * HISTORY_H));
			historyStrength.emplace_back(strengthPoint);
		}
		historyMassLine.emplace_back(HISTORY_X + HISTORY_W,
																 HISTORY_Y + HISTORY_H -
																		 double(lastMyUpdate[U"mass"].get<int>()) /
																				 double(maxMass) * HISTORY_H);
		historyStrength.emplace_back(HISTORY_X + HISTORY_W, HISTORY_Y + HISTORY_H);
		historyStrengthPolygon = Polygon(historyStrength);
	}
	void draw() {
		Rect(HISTORY_X, HISTORY_Y, HISTORY_W, HISTORY_H)
				.drawFrame(0, 1, shadowColor);

		historyStrengthPolygon.draw(paintColor1);
		historyMassLine.draw(2, textColor2);
	}
} history;

}	 // namespace Game
