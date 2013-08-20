#ifndef __BOOL_STATE__
#define __BOOL_STATE__

#include "Definitions.h"
#include "Object.h"

class BoolState : public Object {
public:
	BoolState(float inside = 0.0, float sun = 0.0);
	virtual void draw(const mat4& trans, const mat4& skew) override;
private:
	float isInside;
	float isSun;
};

#endif