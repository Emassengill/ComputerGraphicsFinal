#include "Node.h"
#include "RenderGraph.h"

#define FOR_EACH_CHILD for(Node** child = _children.get(), ** end = child + _children.size(); child < end; ++child)

//PUBLIC

Node::~Node() { FOR_EACH_CHILD delete *child; }

void Node::draw(	RenderGraph& context, const mat4& trans, const mat4& skew,
					bool dynamic, bool drawing										)
{
	FOR_EACH_CHILD (*child)->draw(context, trans, skew, dynamic, drawing);
}

void Node::draw(RenderGraph& context, const mat4& trans, const mat4& skew, bool dynamic) {
	draw(context, trans, skew, dynamic, false);
}

void Node::animate(float number) { FOR_EACH_CHILD (*child)->animate(number); }

GraphInfo Node::getInfo() const {
	GraphInfo info = GraphInfo();
	FOR_EACH_CHILD info.mergeWith( (*child)->getInfo() );
	return info;
}

void Node::addChild(Node& node) { _children.push_back(&node); }