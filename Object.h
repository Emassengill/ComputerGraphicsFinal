#ifndef __OBJECT__
#define __OBJECT__

#include "Definitions.h"

class RenderGraph;

class Object {
public:
	virtual ~Object() { }
	virtual void draw(const RenderGraph& context, const mat4& trans, const mat4& skew) const = 0;
};

#endif