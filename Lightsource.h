#ifndef __POINT_LIGHT__
#define __POINT_LIGHT__

#include "Object.h"

class Lightsource : public Object {
public:
	Lightsource(const char* targ, bool dir);
	virtual void draw(const RenderGraph& context, const mat4& trans, const mat4& skew) const override;
	const char* const target() const;
	bool directional() const;
private:
	const char* const _target;
	const bool _directional;
};

#endif