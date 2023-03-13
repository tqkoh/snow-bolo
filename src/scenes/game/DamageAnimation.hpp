#pragma once

#include <Siv3D.hpp>

namespace Game {

class damageAnimation {
public:
	String id;
	int y = 0, x = 0;
	int damage = 0;
	int frame = 0;
	bool finish = false;
	damageAnimation(int y, int x, int damage, int frame, String id, bool finish)
			: y(y), x(x), damage(damage), frame(frame), id(id), finish(finish) {}
	int drawDamage(int oY, int oX) const {
		if(finish) {
			(*fontMedium)(damage).drawAt(x - oX, y - oY + 5, damageColor);
		} else {
			(*fontSmall)(damage).drawAt(x - oX, y - oY + 5, damageColor);
		}
		return ::frame > frame + ANIMATION_DAMAGE_LENGTH;
	}
};
const bool operator<(const damageAnimation& lhs, const damageAnimation& rhs) {
	if(lhs.id == rhs.id) {
		if(lhs.frame == rhs.frame) {
			return &lhs < &rhs;
		}
		return lhs.frame < rhs.frame;
	}

	return lhs.id < rhs.id;
}

std::set<damageAnimation> damageAnimations;

}	 // namespace Game
