#ifndef __NODE__
#define __NODE__

#include "MatMath.h"
#include "Container.h"

class LightNode;
class RenderGraph;

class Node {
public:
	virtual ~Node();

	virtual void draw(const RenderGraph& context, const mat4& trans, const mat4& skew,
		bool dynamic, bool drawMode);
	void draw(const RenderGraph& context, const mat4& trans = MatMath::ID,
		const mat4& skew = MatMath::ID, bool dynamic = false);

	virtual void animate(float number);

	virtual LightNode* getLight();
	virtual void addChild(Node& node);
private:
	Container<Node*> _children;
};

#endif