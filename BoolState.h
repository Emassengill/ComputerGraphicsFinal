#ifndef __BOOL_STATE__
#define __BOOL_STATE__

#include "Definitions.h"
#include "Object.h"

class RenderGraph;

class BoolState : public Object {
public:
	BoolState(GLuint inside = 0, GLuint sun = 0.0);
	virtual void draw(const RenderGraph& context, const mat4& trans, const mat4& skew) const override;
private:
	const GLuint _isInside;
	const GLuint _isSun;
};

//PUBLIC

inline
BoolState::BoolState(GLuint inside, GLuint sun) : _isInside(inside), _isSun(sun) { }

#endif