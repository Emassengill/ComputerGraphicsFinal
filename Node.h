#ifndef __NODE__
#define __NODE__

#include "MatMath.h"
#include "Container.h"
#include "GraphInfo.h"

class RenderGraph;

class Node {
public:
	virtual ~Node();
	virtual void draw(RenderGraph& context, const mat4& trans, const mat4& skew,
		bool dynamic, bool drawing);
	void draw(RenderGraph& context, const mat4& trans = MatMath::ID,
		const mat4& skew = MatMath::ID, bool dynamic = false);
	virtual void animate(float number);
	virtual GraphInfo getInfo() const;
	virtual void addChild(Node& node);
private:
	Container<Node*> _children;
};

#endif