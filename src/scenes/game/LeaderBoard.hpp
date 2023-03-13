#pragma once

#include <Siv3D.hpp>
#include "Other.hpp"

namespace Game {

class LeaderBoard {
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

	std::vector<leaderBoardRow> leaderBoardRows;

public:
	void clear() { leaderBoardRows.clear(); }
	void place(String id, String name, int mass) {
		leaderBoardRows.emplace_back(id, name, mass);
	}
	void draw() {
		(*fontSmall)(U"Leaderboard").draw(LEADERBOARD_X + 1, 10, shadowColor);
		(*fontSmall)(U"Leaderboard").draw(LEADERBOARD_X, 10, textColor2);

		for(int i = 0; i < leaderBoardRows.size(); ++i) {
			auto e = leaderBoardRows[i];
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
} leaderBoard;

}	 // namespace Game
