#pragma once

#include <Siv3D.hpp>

class Title {
	Font font;
	TextEditState name;

public:
	Title() : font(10) {}
	~Title() {}
	void init() {}
	void end() {}
	int update(WebSocket& ws) {	 // 1: start game
		if(KeyEnter.down()) {
			JSON json, args;
			args[U"name"] = name.text;
			json[U"Method"] = U"join";
			json[U"Args"] = args;
			ws.SendText(json.formatUTF8Minimum());

			return 1;
		}
		return 0;
	}
	void draw() {
		SimpleGUI::TextBox(name, Vec2(10, 10), 100);
		return;
	}
};
