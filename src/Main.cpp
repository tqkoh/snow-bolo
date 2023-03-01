// #include <Siv3D.hpp>
// #include "scenes/Game.hpp"
// #include "scenes/Title.hpp"

enum MainState {
	TITLE,
	GAME,

	MainState_NUM
};

#include <Siv3D.hpp>
#include "lib/WebSocket.hpp"
#include "scenes/Game.hpp"
#include "scenes/Title.hpp"

EM_JS(int, GetCanvasWidth, (), { return canvas.width; });
EM_JS(int, GetCanvasHeight, (), { return canvas.height; });

void Main() {
	Console << U"Hello World!";

	WebSocket webSocket("ws://snowball-server.tqk.trap.show/api/ws");
	// ws-test.tqk.trap.show/api/ws

	{
		JSON json;
		json[U"msg"] = U"Hello World!";
		webSocket.SendText(json.formatUTF8Minimum());
	}

	Scene::SetBackground(ColorF{0.8, 0.9, 1.0});

	std::map<String, s3d::Point> m_users;
	String uid;

	Title title;
	Game game;
	MainState state = TITLE;	// TITLE;
	Window::Resize(384, 216);

	// tmp
	const Font font(30, Typeface::Regular, FontStyle::Bitmap);
	String text = U"asdf";
	constexpr Rect area{10, 10, 100, 100};

	while(System::Update()) {
		TextInput::UpdateText(text);
		for(auto e : Keyboard::GetAllInputs()) {
			Print << e;
		}
		const String editingText = TextInput::GetEditingText();
		area.draw(ColorF{0.3});
		font(text + U'|' + editingText).draw(area.pos);
		// font(U"Hello, Siv3D!🚀").drawAt(Scene::Center(), Palette::Black);
		switch(state) {
			case TITLE:
				if(KeyEnter.down()) {
					state = GAME;
				}

				if(title.update(webSocket) == 1) {
					state = GAME;
					title.end();
					game.init();

					Print << U"game init";
				}
				title.draw();
				break;
			case GAME:
				Circle{Cursor::Pos(), 40}.draw(ColorF{1, 0, 0, 0.5});

				if(game.update(webSocket) == 1) {
					state = TITLE;
					title.init();
					Print << U"game end";
				}
				game.draw();
				break;
			default:

				// state = TITLE;
				break;
		}
		while(webSocket.hasReceivedText()) {
			JSON json = JSON::Parse(
					Unicode::FromUTF8(webSocket.getReceivedTextAndPopFromBuffer()));
			// if(json[U"type"].getString() == U"whoami") {
			// 	uid = json[U"uid"].getString();
			// } else if(json[U"uid"].getString() != uid) {
			// 	auto uid = json[U"uid"].getString();
			// 	auto cursor = json[U"cursor"].getOpt<s3d::Point>();
			// 	if(cursor) {
			// 		m_users[uid] = cursor.value();
			// 	}

			// 	if(json[U"type"].getString() == U"close") {
			// 		m_users[uid].clear();
			// 	}
			// }
		}	 // 		switch(state) {
			 // 			case TITLE:
			 // 				if(KeyEnter.down()) {
			 // 					state = GAME;
			 // 				}

		// Circle{Cursor::Pos(), 40}.draw(ColorF{1, 0, 0, 0.5});

		// for(const auto& user : m_users)
		// 	Circle{user.second, 40}.draw(ColorF{1, 0, 0, 0.25});

		if(KeyC.down()) {
			JSON json;
			json[U"cursor"] = Cursor::Pos();
			webSocket.SendText(json.formatUTF8Minimum());
		}

		if(KeyA.down()) {
			JSON json;
			json[U"msg"] = U"Key A";
			webSocket.SendText(json.formatUTF8Minimum());
		}
	}
}
