#include "LightNode.h"
#include "Lightsource.h"

//PUBLIC

LightNode::LightNode(Lightsource& light, const mat4& trans, mat4 (*anim)(float))
	: ObjectNode(new Lightsource(light), trans, MatMath::ID, anim), _retrieved(false)
{ }

LightNode::~LightNode() { delete _object; }

//override
LightNode* LightNode::getLight() {
	if (_retrieved) {
		return Node::getLight();
	} else {
		_retrieved = true;
		return this;
	}
}

mat4 LightNode::genProjection() const {
	//if (directional) return Frustum(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 10.0f);
	//else return Perspective(
	return Frustum(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 10.0f); //stub
}

bool LightNode::isDirectional() const { return ((const Lightsource*)_object)->directional(); }

void LightNode::reset() { _retrieved = false; }