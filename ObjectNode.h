#ifndef __OBJECT_NODE__
#define __OBJECT_NODE__

#include "Definitions.h"
#include "Node.h"

class Object;

class ObjectNode : public Node {
public:
	ObjectNode(Object* obj = nullptr, const mat4& trans = MatMath::ID, const mat4& skew = MatMath::ID,
		mat4 (*animFunc)(float) = nullptr);
	void transform(const mat4& trans, const mat4& skew = MatMath::ID);
	virtual void draw(const RenderGraph& context, const mat4& trans, const mat4& skew, bool dynamic,
		bool drawMode)
		override;
	virtual void animate(float number) override;
	const mat4& getTrans() const;
	const mat4& getSkew() const;
	vec4 genTranslation() const;
protected:
	const Object* _object;
private:
	mat4 _localTrans;
	mat4 _localSkew;
	mat4 (*_animation)(float);
	bool _cached;
};

#endif