#ifndef __POINT_LIGHT__
#define __POINT_LIGHT__

#include "Object.h"

class PointLight : public Object {
public:
	PointLight(const char* targ);
	virtual void draw(const mat4& trans, const mat4& skew) override;
private:
	const char* target;
};

#endif