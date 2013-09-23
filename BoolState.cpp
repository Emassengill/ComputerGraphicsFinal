#include "BoolState.h"
#include "RenderGraph.h"

//PUBLIC

//override
void BoolState::draw(const RenderGraph& context, const mat4& trans, const mat4& skew) const {
	if (!context.shadowMapping) {
		glUniform1ui(context.isInside_loc, _isInside);
		glUniform1ui(context.isSun_loc, _isSun);
	}
}