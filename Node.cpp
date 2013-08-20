#include "Node.h"

//PUBLIC

Node::Node(Node* sub, Node* next) : child(sub), sibling(next), lastChild(sub) { }

Node::~Node() {
	if (child != nullptr) delete child;
	if (sibling != nullptr) delete sibling;
}

void Node::transform(const mat4& trans, const mat4& skew) {
	inheritTrans(trans, skew); }

void Node::draw(const mat4& trans, const mat4& skew, bool dynamic, bool shadowMap) {
	inheritDraw(trans, skew, dynamic, shadowMap);
}

void Node::animate(float number, bool parentCall) {
	inheritAnim(number, parentCall);
}

void Node::addChild(Node* node) {
	if (lastChild == nullptr) lastChild = child = node;
	else lastChild = lastChild->addSibling(node);
}

//PROTECTED

void Node::inheritTrans(const mat4& trans, const mat4& skew) {
	child->transform(trans, skew);
	sibling->transform(trans, skew);
}

void Node:: inheritDraw(const mat4& trans, const mat4& skew,
	bool dynamic, bool shadowMap) {
	child->draw(trans, skew, dynamic, shadowMap);
	sibling->draw(trans, skew, dynamic, shadowMap);
}

void Node::inheritAnim(float number, bool parentCall) {
	child->animate(number, parentCall);
	if (parentCall) sibling->animate(number, parentCall);
}

Node* Node::getChild() { return child; }

Node* Node::getSibling() { return sibling; }

Node* Node::addSibling(Node* node) { if (sibling == nullptr) sibling = node; return sibling; }