#pragma once

#include <Siv3D.hpp>

namespace Game {

class Chat {
	struct chatMessage {
		String message;
		int timestamp;
		chatMessage(String message, int timestamp)
				: message(message), timestamp(timestamp) {}
	};

	std::deque<chatMessage> chatMessages;

public:
	Chat() {}
	void push(String s, int frame) { chatMessages.emplace_back(s, frame); }
	void draw() {
		while(chatMessages.size()) {
			auto&& e = chatMessages.front();
			if(e.timestamp + CHAT_REMAIN < frame) {
				chatMessages.pop_front();
			} else {
				break;
			}
		}
		while(chatMessages.size() > 10) {
			chatMessages.pop_front();
		}
		{
			int i = 0, n = chatMessages.size();
			for(auto e : chatMessages) {
				++i;
				(*fontSmall)(e.message).draw(11, resolution.y - 90 - (n - 1 - i) * 20,
																		 shadowColor);
				(*fontSmall)(e.message).draw(10, resolution.y - 90 - (n - 1 - i) * 20,
																		 textColor1);
			}
		}
	}
} chat;

}	 // namespace Game
