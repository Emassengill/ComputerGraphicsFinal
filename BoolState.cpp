#include "BoolState.h"
#include "Global.h"

BoolState::BoolState(float inside, float sun) : isInside(inside), isSun(sun) { }

//override
void BoolState::draw(const mat4& trans, const mat4& skew) {
	glUniform1f(Global::isInside_loc, isInside);
	glUniform1f(Global::isSun_loc, isSun);
}