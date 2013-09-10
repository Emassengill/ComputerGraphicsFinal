#include "BoolState.h"
#include "RenderGraph.h"

BoolState::BoolState(GLuint inside, GLuint sun) : _isInside(inside), _isSun(sun) { }

//override
void BoolState::draw(const RenderGraph& context, const mat4& trans, const mat4& skew) const {
	glUniform1ui(context.isInside_loc, _isInside);
	glUniform1ui(context.isSun_loc, _isSun);
}