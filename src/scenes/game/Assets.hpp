#pragma once

#include <Siv3D.hpp>

namespace Game {

std::unique_ptr<Font> fontSmall;
std::unique_ptr<Font> fontMedium;
std::unique_ptr<Font> fontLarge;

std::unique_ptr<Texture> backImage;
std::unique_ptr<Texture> ballImage;
std::unique_ptr<Texture> prepareImage;
std::unique_ptr<Texture> katasaDekasaImage;
std::unique_ptr<Texture> miniMapImage;
std::unique_ptr<Texture> resultImage;
std::unique_ptr<Audio> themeAudio;

void loadAssets() {
	fontSmall =
			std::make_unique<Font>(FONT_SIZE_SMALL, FONT_PATH, FontStyle::Bitmap);
	fontMedium =
			std::make_unique<Font>(FONT_SIZE_MEDIUM, FONT_PATH, FontStyle::Bitmap);
	fontLarge =
			std::make_unique<Font>(FONT_SIZE_LARGE, FONT_PATH, FontStyle::Bitmap);
	backImage = std::make_unique<Texture>(U"assets/images/back.png");
	ballImage = std::make_unique<Texture>(U"assets/images/ball.png");
	prepareImage = std::make_unique<Texture>(U"assets/images/prepare.png");
	katasaDekasaImage =
			std::make_unique<Texture>(U"assets/images/katasa_dekasa.png");
	miniMapImage = std::make_unique<Texture>(U"assets/images/minimap.png");
	resultImage = std::make_unique<Texture>(U"assets/images/result.png");

	themeAudio = std::make_unique<Audio>(U"assets/sounds/snow-bolo_theme.mp3",
																			 Arg::loopBegin = THEME_LOOP_BEGIN,
																			 Arg::loopEnd = THEME_LOOP_END);
	// Arg::loopBegin = 1741510, Arg::loopEnd = 2799910);
}

}	 // namespace Game
