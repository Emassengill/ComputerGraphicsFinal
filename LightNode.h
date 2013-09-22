#ifndef __LIGHT_NODE__
#define __LIGHT_NODE__

#include "Global.h"
#include "ObjectNode.h"
#include "Lightsource.h"

class LightNode: public ObjectNode {
public:
	LightNode(Lightsource& light, const mat4& trans = MatMath::ID,
		mat4 (*anim)(float) = nullptr);
	virtual ~LightNode() override;
	virtual void draw(RenderGraph& context, const mat4& trans, const mat4& skew,
		bool dynamic, bool drawing) override;
	virtual GraphInfo getInfo() const override;
	mat4 genCamera(const mat4& viewer) const;
	mat4 genProjection() const;
	bool isDirectional() const;
	void reset();
private:
	bool _retrieved;
	mat4 _lastTrans;
};

//PUBLIC

inline
LightNode::LightNode(Lightsource& light, const mat4& trans, mat4 (*anim)(float))
	: ObjectNode(new Lightsource(light), trans, MatMath::ID, anim), _retrieved(false), _lastTrans(1.0)
{ }

inline
LightNode::~LightNode() { delete _object; }

inline
mat4 LightNode::genCamera(const mat4& viewer) const {
	if (isDirectional()) {
		vec4 direction = getTrans() * MatMath::yAXIS;
		if (cached()) direction = _lastTrans * direction;
		return LookAt(MatMath::ORIGIN, -direction, MatMath::yAXIS);
	} else {
		vec4 camTrans = genTranslation4(viewer);
		return LookAt(genTranslation(), -camTrans, MatMath::yAXIS);
	}
}

inline
mat4 LightNode::genProjection() const {
	float distance = 21.0f;
	return Ortho(-distance, distance, -distance, distance, -distance, distance); //stub
}

inline
bool LightNode::isDirectional() const { return static_cast<const Lightsource*>(_object)->directional(); }

inline
void LightNode::reset() { _retrieved = false; }

#endif