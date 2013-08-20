#include "PointLight.h"
#include "Global.h"

//PUBLIC

PointLight::PointLight(const char* targ) : target(targ) { }

//override
void PointLight::draw(const mat4& trans, const mat4& skew) {
	GLuint light_loc = glGetUniformLocation(Global::currentProg, target);
	glUniform3f(light_loc, trans[0][3], trans[1][3], trans[2][3]);
}