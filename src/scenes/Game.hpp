#pragma once

#include <Siv3D.hpp>
#include "lib/WebSocket.hpp"

enum GameState {
	ALIVE,
	DEAD,

	GameState_NUM
};

class Game {
	WebSocket ws;

public:
	Game() : ws("ws://snowball-server.tqk.trap.show/api/ws") {}
	~Game() {}
	void init(String name = U"test") {
		JSON json, args;
		args[U"name"] = name;
		json[U"Method"] = U"init";
		json[U"Args"] = args;
		ws.SendText(json.formatUTF8Minimum());
	}
	int update() {
		if(KeyA.down()) {
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
