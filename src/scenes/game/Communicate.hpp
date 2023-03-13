#pragma once

#include <Siv3D.hpp>
#include "lib/WebSocket.hpp"

#include "Result.hpp"

namespace Game {

std::unique_ptr<WebSocket> ws;

void receive() {
	int dead = 0;
	String received;
	while(ws->hasReceivedText()) {
		received = Unicode::FromUTF8(ws->getReceivedTextAndPopFromBuffer());
		// 0&&printf("received: %s\n", received.narrow().c_str());
		// try {
		if(received == U"") {
			lastUpdate = defaultUpdate;
			lastUpdate[U"timestamp"] = frame;
			continue;
		}
		JSON json = JSON::Parse(received, AllowExceptions::Yes);
		if(!json) {
			0 && printf("parse failed: %s\n", received.narrow().c_str());
			lastUpdate = defaultUpdate;
			lastUpdate[U"timestamp"] = frame;
		}
		if(json[U"method"] == U"joinAccepted") {
			myId = json[U"args"][U"id"].get<String>();
			id = myId;
			spectateMode = OFF;
		} else if(json[U"method"] == U"dead") {
			dead = lastMyUpdate[U"strength"].get<int>();
			kills = json[U"args"][U"kills"].get<int>();
			resultShowing = true;
			spectateMode = MAP;
			deadFrame = frame;

			result.prepareDraw();
		} else if(json[U"method"] == U"update") {
			lastUpdate = json[U"args"];
			lastUpdate[U"timestamp"] = frame;
		} else if(json[U"method"] == U"message") {
			String message = json[U"args"][U"message"].get<String>();
			chat.push(message, frame);
		}
		// } catch(...) {
		// 	0&&printf("parse failed: %s\n",
		// received.narrow().c_str());
		// }
	}
	if(dead) {
		lastMyUpdate[U"damage"] = dead;
		lastMyUpdate[U"strength"] = 0;
	}
}

void sendJoin() {
	JSON json, input;
	input[U"name"] = name.text;
	json[U"method"] = U"join";
	json[U"Args"] = input;
	ws->SendText(json.formatUTF8Minimum());
}

void sendInput() {
	if(windowVisible) {
		JSON json, input;
		auto p = Cursor::Pos() / scaling;
		input[U"W"] = keys["W"];
		input[U"A"] = keys["A"];
		input[U"S"] = keys["S"];
		input[U"D"] = keys["D"];
		input[U"left"] = MouseL.pressed();
		input[U"right"] = MouseR.pressed();
		input[U"dy"] = p.y - center.y;
		input[U"dx"] = p.x - center.x;

		json[U"method"] = U"input";
		json[U"args"] = input;

		// look at the md #2
		if(MouseL.pressed() && frame % SEND_INPUT_PER == 0 ||
			 MouseR.pressed() && frame % SEND_INPUT_PER == 0 ||
			 keys["W"] != previnput[U"W"].get<bool>() ||
			 keys["A"] != previnput[U"A"].get<bool>() ||
			 keys["S"] != previnput[U"S"].get<bool>() ||
			 keys["D"] != previnput[U"D"].get<bool>() ||
			 MouseL.pressed() != previnput[U"left"].get<bool>() ||
			 MouseR.pressed() != previnput[U"right"].get<bool>()) {
			ws->SendText(json.formatUTF8Minimum());
			previnput = input;

			// 0&&printf("time: %lld", std::time(nullptr));
		}
	}
}

}	 // namespace Game
