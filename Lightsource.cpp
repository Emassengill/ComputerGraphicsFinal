#include "Lightsource.h"

//override
void Lightsource::draw(const RenderGraph& context, const mat4& trans, const mat4& skew) const {
	if (!context.shadowMapping) {
		GLuint light_loc = glGetUniformLocation(context.program, _target);
		vec4 temp = _directional ? (normalize(trans * MatMath::yAXIS)) : (trans * MatMath::ORIGIN);
		glUniform3f(light_loc, temp[0], temp[1], temp[2]);
	}
}