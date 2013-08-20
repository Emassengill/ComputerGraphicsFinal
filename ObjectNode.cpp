#include "ObjectNode.h"
#include "Object.h"

//PUBLIC

ObjectNode::ObjectNode(Object* obj, Node* sub, Node* next, const mat4& trans, const mat4& skew,
	mat4 (*animFunc)(float))
	: Node(sub, next), object(obj), localTrans(trans), localSkew(skew), animation(animFunc), cached(false)
{ }

//PROTECTED

//override
void ObjectNode::inheritTrans(const mat4& trans, const mat4& skew) {
	localTrans = trans * localTrans;
	localSkew = skew * localSkew;
}

//override
void ObjectNode::inheritDraw(const mat4& trans, const mat4& skew, bool dynamic, bool shadowMap) {
	//Recalc flags Global::whether the node has stored the result of its matrix multiplication.
	//On the first call of draw(), if a node is not animated, it stores the result of its
	//local multiplication. On future calls, instead of recalculating the same product, it
	//uses the stored result. If the node is animated, or the child of an animated node, then
	//it continues to perform multiplications every frame, ensuring that nodes participating
	//in an animation do not become frozen.
	if (dynamic || animation != nullptr) {
		const mat4 drawTrans = trans * localTrans;
		const mat4 drawSkew = skew * localSkew;
		if (object != nullptr) {
			object->draw(drawTrans, drawSkew);
		}
		Node* tempChild = Node::getChild();
		if (tempChild != nullptr) {
			tempChild->draw(drawTrans, drawSkew, true, shadowMap);
		}
	} else if (!cached) {
		localTrans = trans * localTrans;
		localSkew = skew * localSkew;
		cached = true;
		if (object != nullptr) {
			object->draw(localTrans, localSkew);
		}
		Node* tempChild = Node::getChild();
		if (tempChild != nullptr) {
			tempChild->draw(localTrans, localSkew, dynamic, shadowMap);
		}
	} else { // if not animated, and already cached
		if (object != nullptr) {
			object->draw(localTrans,localSkew);
		}
		Node* tempChild = Node::getChild();
		if (tempChild != nullptr) {
			tempChild->draw(localTrans, localSkew, dynamic, shadowMap);
		}
	}
	Node* tempSibling = Node::getSibling();
	if (tempSibling != nullptr) {
		tempSibling->draw(trans, skew, dynamic, shadowMap);
	}
}

//override
void ObjectNode::inheritAnim(float number, bool parentCall) {
	if (animation != nullptr) {
		const mat4 animTrans = animation(number);
		inheritTrans(animTrans, animTrans);
	}
	Node* tempChild = Node::getChild();
	if (tempChild != nullptr) tempChild->animate(number);
	if (parentCall) {
		Node* tempSibling = Node::getSibling();
		if (tempSibling != nullptr) tempSibling->animate(number);
	}
}

inline
const mat4& ObjectNode::getTrans() { return localTrans; }

inline
const mat4& ObjectNode::getSkew() { return localSkew; }