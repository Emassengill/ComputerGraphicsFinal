#ifndef __NODE__
#define __NODE__

#include "MatMath.h"
#include "Container.h"
#include "GraphInfo.h"

class LightNode;
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

#define FOR_EACH_CHILD for(Node** child = _children.get(), ** end = child + _children.size(); child < end; ++child)

//PUBLIC

inline
Node::~Node() { FOR_EACH_CHILD delete *child; }

inline
void Node::draw(	RenderGraph& context, const mat4& trans, const mat4& skew,
					bool dynamic, bool drawing										)
{
	FOR_EACH_CHILD (*child)->draw(context, trans, skew, dynamic, drawing);
}
inline
void Node::draw(RenderGraph& context, const mat4& trans, const mat4& skew, bool dynamic) {
	draw(context, trans, skew, dynamic, false);
}

inline
void Node::animate(float number) { FOR_EACH_CHILD (*child)->animate(number); }

inline
GraphInfo Node::getInfo() const {
	GraphInfo info = GraphInfo();
	FOR_EACH_CHILD info.mergeWith( (*child)->getInfo() );
	return info;
}

inline
void Node::addChild(Node& node) { _children.push_back(&node); }

#endif