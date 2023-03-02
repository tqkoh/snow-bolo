#pragma once

#include <Siv3D.hpp>

namespace Title {
std::unique_ptr<Texture> titleImage;
void init() {
	titleImage = std::make_unique<Texture>(U"assets/images/title.png");
}
void end() {}
int update() {
	if(KeyEnter.down()) {
		return 1;
	}
	return 0;
}
void draw() {
	titleImage->draw(0, 0);
}
}	 // namespace Title
