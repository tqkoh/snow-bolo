#pragma once

#include <Siv3D.hpp>

namespace Game {

struct chatMessage {
	String message;
	int timestamp;
	chatMessage(String message, int timestamp)
			: message(message), timestamp(timestamp) {}
};

std::deque<chatMessage> chatMessages;

}	 // namespace Game
