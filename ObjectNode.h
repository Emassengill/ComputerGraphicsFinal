#ifndef __OBJECT_NODE__
#define __OBJECT_NODE__

#include "Definitions.h"
#include "Node.h"

class Object;

class ObjectNode : public Node {
public:
	ObjectNode(Object* obj = nullptr, Node* sub = nullptr, Node* next = nullptr,
		const mat4& trans = MatMath::ID, const mat4& skew = MatMath::ID, mat4 (*animFunc)(float) = nullptr);
protected:
	virtual void inheritTrans(const mat4& trans, const mat4& skew) override;
	virtual void inheritDraw(const mat4& trans, const mat4& skew, bool dynamic, bool shadowMap) override;
	virtual void inheritAnim(float number, bool parentCall) override;
	const mat4& getTrans();
	const mat4& getSkew();
private:
	Object* object;
	mat4 localTrans;
	mat4 localSkew;
	mat4 (*animation)(float);
	bool cached;
};

#endif