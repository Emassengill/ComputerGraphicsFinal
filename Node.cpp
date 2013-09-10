#include "Node.h"

//PUBLIC

Node::~Node() {
	for (Node** child = _children.get(), ** end = child + _children.size(); child < end; ++child) {
		delete *child;
	}
}

void Node::draw(	const RenderGraph& context, const mat4& trans, const mat4& skew,
					bool dynamic, bool drawMode										)
{
	for (Node** child = _children.get(), ** end = child + _children.size(); child < end; ++child) {
		(*child)->draw(context, trans, skew, dynamic, drawMode);
	}
}
void Node::draw(const RenderGraph& context, const mat4& trans, const mat4& skew, bool dynamic) {
	draw(context, trans, skew, dynamic, false);
}

void Node::animate(float number) {
	for (Node** child = _children.get(), ** end = child + _children.size(); child < end; ++child) {
		(*child)->animate(number);
	}
}

LightNode* Node::getLight()
//Probes the tree to get a pointer to a LightNode. Repeated probing until a nullptr return will retrieve
//all lights in this subtree.
{
	LightNode* temp = nullptr;
	for (	Node** child = _children.get(), ** end = child + _children.size();
			temp == nullptr && child < end; ++child							)
	{
		temp = (*child)->getLight();
	}
	return temp;
}

void Node::addChild(Node& node) {
	_children.push_back(&node);
}