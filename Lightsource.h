#ifndef __POINT_LIGHT__
#define __POINT_LIGHT__

#include "Object.h"
#include "RenderGraph.h"

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

//PUBLIC

inline
Lightsource::Lightsource(const char* const targ, bool dir) : _target(targ), _directional(dir) { }

inline
const char* const Lightsource::target() const { return _target; }

inline
bool Lightsource::directional() const { return _directional; }

#endif