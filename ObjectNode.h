#ifndef __OBJECT_NODE__
#define __OBJECT_NODE__

#include "Definitions.h"
#include "Node.h"
#include "Object.h"

class ObjectNode : public Node {
public:
	ObjectNode(Object* obj = nullptr, const mat4& trans = MatMath::ID, const mat4& skew = MatMath::ID,
		mat4 (*animFunc)(float) = nullptr);
	void transform(const mat4& trans, const mat4& skew = MatMath::ID);
	virtual void draw(RenderGraph& context, const mat4& trans, const mat4& skew, bool dynamic,
		bool drawing) override;
	virtual void animate(float number) override;
	const mat4& getTrans() const;
	const mat4& getSkew() const;
	vec4 genTranslation() const;
	vec4 genRotation() const;
	bool animated() const;
	bool cached() const;
protected:
	const Object* _object;
private:
	mat4 _localTrans;
	mat4 _localSkew;
	mat4 (*_animation)(float);
	bool _cached;
};

//PUBLIC

inline
ObjectNode::ObjectNode(Object* obj, const mat4& trans, const mat4& skew, mat4 (*animFunc)(float))
	: _object(obj), _localTrans(trans), _localSkew(skew), _animation(animFunc), _cached(false)
{ }

inline
const mat4& ObjectNode::getTrans() const { return _localTrans; }

inline
const mat4& ObjectNode::getSkew() const { return _localSkew; }

inline
vec4 ObjectNode::genTranslation() const {
	return vec4(_localTrans[0][3], _localTrans[1][3], _localTrans[2][3], 1.0f);
}

inline
bool ObjectNode::animated() const { return static_cast<bool>(_animation); }

inline
bool ObjectNode::cached() const { return _cached; }

#endif