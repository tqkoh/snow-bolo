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
	Scene::Resize(resolution * scaling);
	Window::Resize(resolution * scaling);

	Scene::SetBackground(backColor);

	const ScopedRenderStates2D renderState(SamplerState::ClampNearest);
	RenderTexture renderTexture(resolution);

	MainState state = TITLE;	// TITLE;
	Title::init();
	Game::load();

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
			std::this_thread::sleep_for(std::chrono::milliseconds(int(rest * 1000)));
			// emscripten_sleep(rest * 1000);
		}
	}
}
