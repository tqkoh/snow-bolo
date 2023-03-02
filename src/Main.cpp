// #include <Siv3D.hpp>
// #include "scenes/Game.hpp"
// #include "scenes/Title.hpp"

enum MainState {
	TITLE,
	GAME,

	MainState_NUM
};

#include <Siv3D.hpp>
#include "Global.hpp"
#include "lib/WebSocket.hpp"
#include "scenes/Game.hpp"
#include "scenes/Title.hpp"

EM_JS(int, GetCanvasWidth, (), { return canvas.width; });
EM_JS(int, GetCanvasHeight, (), { return canvas.height; });

void Main() {
	Scene::SetTextureFilter(TextureFilter::Nearest);
	// Scene::SetResizeMode(ResizeMode::Keep);
	// Scene::Resize(resolution * scaling);
	Window::Resize(resolution * scaling);

	Scene::SetBackground(backColor);

	const ScopedRenderStates2D renderState(SamplerState::ClampNearest);
	RenderTexture renderTexture(resolution);

	MainState state = TITLE;	// TITLE;
	Title::init();

	Stopwatch calcTime(StartImmediately::Yes);
	while(System::Update()) {
		++frame;
		calcTime.restart();
		renderTexture.clear(backColor);
		{
			const ScopedRenderTarget2D renderTarget(renderTexture);

			switch(state) {
				case TITLE:
					if(KeyEnter.down()) {
						state = GAME;
					}

					if(Title::update() == 1) {
						state = GAME;
						Title::end();
						Game::init();
					}
					Title::draw();
					break;

				case GAME:
					if(Game::update() == 1) {
						state = TITLE;
						Title::init();
					}
					Game::draw();
					break;
				default:

					// state = TITLE;
					break;
			}
		}

		renderTexture.scaled(scaling).draw();

		double rest = 1. / 60 - calcTime.sF();
		if(rest > 0) {
			// std::this_thread::sleep_for(std::chrono::milliseconds(int(rest *
			// 1000)));
			emscripten_sleep(rest * 1000);
		}
	}
}

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

// 	while(System::Update()) {
// 		renderTexture.clear(bgColor);

// 		{
// 			const ScopedRenderTarget2D renderTarget(renderTexture);

// 			RectF(resolution * 0.5 - Vec2(20, 20), 40, 40)
// 					.rotated(Scene::Time() * 0.3)
// 					.draw(Palette::Black);
// 			Circle(Cursor::PosF() / scaling, 10).draw(ColorF(0.0, 0.5));
// 		}

// 		renderTexture.scaled(scaling).draw();
// 	}
// }

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

void Mains() {
	Scene::SetBackground(ColorF{0.8, 0.9, 1.0});

	const Font font{30};

	const Font emojiFont{60, Typeface::ColorEmoji};
	font.addFallback(emojiFont);

	const Size resolution(128, 72);
	const int scaling = 10;
	Window::Resize(resolution * scaling);

	const ColorF bgColor = Palette::White;
	Scene::SetBackground(bgColor);

	const ScopedRenderStates2D renderState(SamplerState::ClampNearest);
	RenderTexture renderTexture(resolution);

	MainState mainState = TITLE;
	drawHello2 hello2;

	while(System::Update()) {
		renderTexture.clear(ColorF{0.8, 0.9, 1.0});

		{
			ScopedRenderTarget2D renderTarget(renderTexture);

			Game::draw();
			drawHello(font);
			hello2.draw();
			hello2.draw2(&renderTarget);
			hello2.draw3()();
			[&]() { font(U"hello").drawAt(Scene::Center() / 16, Palette::Purple); }();
			hello2.setTarget(&renderTarget);
			hello2.draw();

			// Put a text in the middle of the screen
			font(U"Hello, Siv3D!🚀").drawAt(Scene::Center() / 9, Palette::Red);
			font(U"Hello, Siv3D!🚀").draw(Scene::Center() / 10, Palette::Black);

			// Draw a red transparent circle that follows the mouse cursor
			Circle{Cursor::Pos(), 40}.draw(ColorF{1, 0, 0, 0.5});

			// When [A] key is down
			if(KeyA.down()) {
				// Print a randomly selected text
				Print << Sample({U"Hello!", U"こんにちは", U"你好", U"안녕하세요?"});
			}
		}

		renderTexture.scaled(scaling).draw();
	}
}
