#include <Siv3D.hpp>
#include "WebSocket.hpp"

void Main() {
	// Set background color to sky blue
	Scene::SetBackground(ColorF{0.8, 0.9, 1.0});

	WebSocket ws("ws://snowball-server.tqk.trap.show/api/ws");

	{
		JSON json, args;
		args[U"message"] = U"Hello World!";
		json[U"Method"] = U"message";
		json[U"Args"] = args;
		ws.SendText(json.formatUTF8Minimum());
	}

	// Create a new font
	const Font font{60};

	// Create a new emoji font
	const Font emojiFont{60, Typeface::ColorEmoji};

	// Set emojiFont as a fallback
	font.addFallback(emojiFont);

	// Create a texture from an image file
	const Texture texture{U"example/windmill.png"};

	// Create a texture from an emoji
	const Texture emoji{U"🐈"_emoji};

	// Coordinates of the emoji
	Vec2 emojiPos{300, 150};

	// Print a text
	Print << U"Push [A] key";

	while(System::Update()) {
		while(ws.hasReceivedText()) {
			// JSON json = JSON::Parse(
			// 		Unicode::FromUTF8(ws.getReceivedTextAndPopFromBuffer()));
			Print << Unicode::FromUTF8(ws.getReceivedTextAndPopFromBuffer());
		}

		// Draw a texture
		texture.draw(200, 200);

		// Put a text in the middle of the screen
		font(U"Hello, Siv3D!🚀").drawAt(Scene::Center(), Palette::Black);

		// Draw a texture with animated size
		emoji.resized(100 + Periodic::Sine0_1(1s) * 20).drawAt(emojiPos);

		// Draw a red transparent circle that follows the mouse cursor
		Circle{Cursor::Pos(), 40}.draw(ColorF{1, 0, 0, 0.5});

		// When [A] key is down
		if(KeyA.down()) {
			// Print a randomly selected text
			Print << Sample({U"Hello!", U"こんにちは", U"你好", U"안녕하세요?"});
		}

		// When [Button] is pushed
		if(SimpleGUI::Button(U"Button", Vec2{640, 40})) {
			// Move the coordinates to a random position in the screen
			emojiPos = RandomVec2(Scene::Rect());
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
	}
}
