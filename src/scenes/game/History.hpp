#pragma once

#include <Siv3D.hpp>

namespace Game {

struct HistoryRecord {
	int frame, mass, strength;
	HistoryRecord(int frame, int mass, int strength)
			: frame(frame), mass(mass), strength(strength) {}
};
std::vector<HistoryRecord> history;
LineString historyMassLine;
std::vector<Vec2> historyStrength;
Polygon historyStrengthPolygon;

}	 // namespace Game
