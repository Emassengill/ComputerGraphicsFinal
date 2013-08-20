#ifndef __OBJECT__
#define __OBJECT__

#include "Definitions.h"

class Object {
public:
	virtual ~Object() { }
	virtual void draw(const mat4& trans, const mat4& skew) = 0;
};

#endif