#pragma once

#include <Siv3D.hpp>
#include "lib/WebSocket.hpp"

enum GameState {
	PREPARE,
	ALIVE,
	DEAD,

	GameState_NUM
};

class Game {
public:
	Game() {}
	~Game() {}
	void init() {}
	int update(WebSocket& ws) {
		if(KeyEnter.down()) {
			return 1;
		}
		while(ws.hasReceivedText()) {
			// JSON json = JSON::Parse(
			// 		Unicode::FromUTF8(ws.getReceivedTextAndPopFromBuffer()));
			Print << Unicode::FromUTF8(ws.getReceivedTextAndPopFromBuffer());
		}

		if(KeyC.down()) {
			JSON json;
			json[U"cursor"] = Cursor::Pos();
			ws.SendText(json.formatUTF8Minimum());
		}

		if(KeyA.down()) {
			JSON json;
			json[U"msg"] = U"Key A!";
			ws.SendText(json.formatUTF8Minimum());
		}
		return 0;
	}
	void draw() {}
};
