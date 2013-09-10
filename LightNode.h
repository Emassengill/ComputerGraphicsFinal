#ifndef __LIGHT_NODE__
#define __LIGHT_NODE__

#include "Global.h"
#include "ObjectNode.h"

class Lightsource;

class LightNode: public ObjectNode {
public:
	LightNode(Lightsource& light, const mat4& trans = MatMath::ID,
		mat4 (*anim)(float) = nullptr);
	virtual ~LightNode() override;
	virtual LightNode* getLight() override;
	mat4 genProjection() const;
	bool isDirectional() const;
	void reset();
private:
	bool _retrieved;
};

#endif