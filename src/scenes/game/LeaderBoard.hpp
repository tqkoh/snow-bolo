#pragma once

#include <Siv3D.hpp>
#include "Other.hpp"

namespace Game {

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

void drawLeaderBoard() {
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

}	 // namespace Game
