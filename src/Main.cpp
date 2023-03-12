// #include <Siv3D.hpp>
// #include "scenes/Game.hpp"
// #include "scenes/Title.hpp"

#include <Siv3D.hpp>
#include "Global.hpp"
#include "lib/WebSocket.hpp"
#include "scenes/Game.hpp"
#include "scenes/Title.hpp"

enum MainState {
	TITLE,
	GAME,

	MainState_NUM
} state = TITLE;

EM_JS(int, GetCanvasWidth, (), { return canvas.width; });
EM_JS(int, GetCanvasHeight, (), { return canvas.height; });
EM_BOOL visibility_callback(int eventType,
														const EmscriptenVisibilityChangeEvent* e,
														void* userData) {
	if(eventType == EMSCRIPTEN_EVENT_VISIBILITYCHANGE) {
		if(!state)
			return 0;
		if(e->hidden) {
			Game::windowVisible = 0;
			JSON json, args;
			args[U"active"] = false;
			json[U"method"] = U"active";
			json[U"args"] = args;
			Game::ws->SendText(json.formatUTF8Minimum());
		} else {
			Game::windowVisible = 1;
			JSON json, args;
			args[U"active"] = true;
			json[U"method"] = U"active";
			json[U"args"] = args;
			Game::ws->SendText(json.formatUTF8Minimum());
		}
	}
	return 0;
}

void Main() {
	emscripten_set_visibilitychange_callback(0, 1, visibility_callback);

	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	Scene::SetTextureFilter(TextureFilter::Nearest);
	// Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(resolution * scaling);
	Window::Resize(resolution * scaling);

	Scene::SetBackground(backColor);

	const ScopedRenderStates2D renderState(SamplerState::ClampNearest);
	RenderTexture renderTexture(resolution);

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
