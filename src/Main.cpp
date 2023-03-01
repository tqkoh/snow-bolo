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
#include "scenes/Scoped.hpp"
#include "scenes/Title.hpp"

EM_JS(int, GetCanvasWidth, (), { return canvas.width; });
EM_JS(int, GetCanvasHeight, (), { return canvas.height; });

void Mains() {
	Console << U"Hello World!";

	// WebSocket webSocket("ws://snowball-server.tqk.trap.show/api/ws");
	// ws-test.tqk.trap.show/api/ws

	// {
	// 	JSON json;
	// 	json[U"msg"] = U"Hello World!";
	// 	webSocket.SendText(json.formatUTF8Minimum());
	// }

	Scene::SetBackground(ColorF{0.8, 0.9, 1.0});

	std::map<String, s3d::Point> m_users;
	String uid;

	Title title;
	Game game;
	MainState state = TITLE;	// TITLE;

	const Size resolution(384, 216);
	const int scaling = 4;
	Window::Resize(resolution * scaling);
	const ScopedRenderStates2D renderState(SamplerState::ClampNearest);
	RenderTexture renderTexture(resolution);

	// tmp

	Font font(30, U"example/font/DotGothic16/DotGothic16-Regular.ttf",
						FontStyle::Bitmap);

	while(System::Update()) {
		renderTexture.clear(ColorF{0.8, 0.9, 1.0});
		{
			const ScopedRenderTarget2D renderTarget(renderTexture);

			font(U"hello").drawAt(Scene::Center() / 10, Palette::Black);
			font(U"hello").draw(Scene::Center() / 9, Palette::Red);
			switch(state) {
				case TITLE:
					if(KeyEnter.down()) {
						state = GAME;
					}

					if(title.update() == 1) {
						state = GAME;
						title.end();
						game.init();

						Print << U"game init";
					}
					title.draw();
					break;
				case GAME:
					if(game.update() == 1) {
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
			// 		switch(state) {
			// 			case TITLE:
			// 				if(KeyEnter.down()) {
			// 					state = GAME;
			// 				}

			// Circle{Cursor::Pos(), 40}.draw(ColorF{1, 0, 0, 0.5});

			// for(const auto& user : m_users)
			// 	Circle{user.second, 40}.draw(ColorF{1, 0, 0, 0.25});
		}

		renderTexture.scaled(scaling).draw();
	}
}

// #include <Siv3D.hpp>	// OpenSiv3D v0.6.6

// void Main() {
// 	Scene::SetTextureFilter(TextureFilter::Nearest);

// 	const Size resolution(128, 72);
// 	const int scaling = 10;
// 	Window::Resize(resolution * scaling);

// 	const ColorF bgColor = Palette::White;
// 	Scene::SetBackground(bgColor);

// 	// RenderTextureを拡大描画時にぼやけないように：
// 	const ScopedRenderStates2D state(SamplerState::ClampNearest);

// 	// 低解像度のレンダーテクスチャ
// 	RenderTexture renderTexture(resolution);
// 	Font font(30);

// 	while(System::Update()) {
// 		renderTexture.clear(bgColor);

// 		{
// 			const ScopedRenderTarget2D renderTarget(renderTexture);

// 			font(U"Hello, Siv3D!🐣").drawAt(Scene::Center() / 4, Palette::Black);
// 			RectF(resolution * 0.5 - Vec2(20, 20), 40, 40)
// 					.rotated(Scene::Time() * 0.3)
// 					.draw(Palette::Black);
// 			Circle(Cursor::PosF() / scaling, 10).draw(ColorF(0.0, 0.5));
// 		}

// 		renderTexture.scaled(scaling).draw();
// 	}
// }

// #include <Siv3D.hpp>

void drawHello(Font font) {
	font(U"hello").drawAt(Scene::Center() / 12, Palette::Blue);
}
class drawHello2 {
	Font font;
	ScopedRenderTarget2D* t;

public:
	drawHello2() : t(nullptr) {
		font = Font(30, U"example/font/DotGothic16/DotGothic16-Regular.ttf",
								FontStyle::Bitmap);
	}
	void draw() { font(U"hello").drawAt(Scene::Center() / 13, Palette::Yellow); }
	void draw2(const ScopedRenderTarget2D* target) {
		target;
		font(U"hello").drawAt(Scene::Center() / 14, Palette::Orange);
	}
	std::function<void()> draw3() {
		return
				[&]() { font(U"hello").drawAt(Scene::Center() / 15, Palette::Cyan); };
	}

	void setTarget(ScopedRenderTarget2D* target) { t = target; }
};

void Main() {
	// Set background color to sky blue
	Scene::SetBackground(ColorF{0.8, 0.9, 1.0});

	// Create a new font
	const Font font{30};

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

	const Size resolution(128, 72);
	const int scaling = 10;
	Window::Resize(resolution * scaling);

	const ColorF bgColor = Palette::White;
	Scene::SetBackground(bgColor);

	renderState = new ScopedRenderStates2D(SamplerState::ClampNearest);

	RenderTexture renderTexture(resolution);

	Game game;
	Title title;
	MainState mainState = TITLE;
	drawHello2 hello2;

	while(System::Update()) {
		renderTexture.clear(ColorF{0.8, 0.9, 1.0});

		{
			renderTarget = new ScopedRenderTarget2D(renderTexture);
			// Draw a texture
			texture.draw(20, 20);

			game.draw();
			drawHello(font);
			hello2.draw();
			hello2.draw2(renderTarget);
			hello2.draw3()();
			[&]() { font(U"hello").drawAt(Scene::Center() / 16, Palette::Purple); }();
			hello2.setTarget(renderTarget);
			hello2.draw();

			// Put a text in the middle of the screen
			font(U"Hello, Siv3D!🚀").drawAt(Scene::Center() / 9, Palette::Red);
			font(U"Hello, Siv3D!🚀").draw(Scene::Center() / 10, Palette::Black);

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

			delete renderTarget;
		}

		renderTexture.scaled(scaling).draw();
	}
	delete renderState;
}
