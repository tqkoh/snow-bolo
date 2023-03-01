#pragma once

#include <Siv3D.hpp>

class Title {
	Font font;
	String name;

public:
	Title() : name(U"test"), font(10) {}
	~Title() {}
	void init() {}
	void end() {}
	int update(WebSocket& ws) {	 // 1: start game
		if(KeyEnter.down()) {
			JSON json, args;
			args[U"name"] = name;
			json[U"Method"] = U"join";
			json[U"Args"] = args;
			ws.SendText(json.formatUTF8Minimum());

			return 1;
		}
		return 0;
	}
	void draw() {
		TextInput::UpdateText(name);
		font(TextInput::GetEditingText()).draw(20, 0, Palette::Black);
		font(U"aaa").draw(20, 20, Palette::Black);
		return;
	}
};
