#pragma once

#include <Siv3D.hpp>

class Title {
	TextEditState nameTextState;

public:
	Title() { init(); }
	~Title() {}
	void init() {
		SimpleGUI::TextBox(nameTextState, Vec2(100, 100), 16, true);
		nameTextState.active = true;
	}
	int update() {	// 1: start game
		if(KeyEnter.down()) {
			return 1;
		}
		return 0;
	}
	void draw() {
		SimpleGUI::TextBox(nameTextState, Vec2(100, 100), 16);
		return;
	}
};
