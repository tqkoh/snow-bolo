#pragma once

#include <Siv3D.hpp>

class TitleC {
public:
	TitleC() {}
	~TitleC() {}
	void init() {}
	void end() {}
	int update() {	// 1: start game
		if(KeyEnter.down()) {
			// JSON json, args;
			// args[U"name"] = name.text;
			// json[U"Method"] = U"join";
			// json[U"Args"] = args;
			// ws.SendText(json.formatUTF8Minimum());

			return 1;
		}
		return 0;
	}
	void draw() {
		// SimpleGUI::TextBox(name, Vec2(10, 10), 100);
		return;
	}
};

namespace Title {
void init() {}
void end() {}
int update() {
	if(KeyEnter.down()) {
		return 1;
	}
	return 0;
}
void draw() {}
}	 // namespace Title
