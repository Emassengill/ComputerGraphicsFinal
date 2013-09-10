#include "Lightsource.h"
#include "RenderGraph.h"

//PUBLIC

Lightsource::Lightsource(const char* const targ, bool dir) : _target(targ), _directional(dir) { }

//override
void Lightsource::draw(const RenderGraph& context, const mat4& trans, const mat4& skew) const {
	GLuint light_loc = glGetUniformLocation(context.program, _target);
	vec4 temp = _directional ? (normalize(trans * MatMath::yAXIS)) : (trans * MatMath::ORIGIN);
	glUniform3f(light_loc, temp[0], temp[1], temp[2]);
}

const char* const Lightsource::target() const { return _target; }

bool Lightsource::directional() const { return _directional; }