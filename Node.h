#ifndef __NODE__
#define __NODE__

#include "MatMath.h"

class Node {
public:
	Node(Node* sub, Node* next);
	virtual ~Node();
	void transform(const mat4& trans, const mat4& skew = MatMath::ID);
	void draw(const mat4& trans = MatMath::ID, const mat4& skew = MatMath::ID,
		bool dynamic = false, bool shadowMap = false);
	void animate(float number = 0.0, bool parentCall = false);
	virtual void addChild(Node* node);
protected:
	virtual void inheritTrans(const mat4& trans, const mat4& skew);
	virtual void inheritDraw(const mat4& trans, const mat4& skew, bool dynamic, bool shadowMap);
	virtual void inheritAnim(float number, bool parentCall);
	Node* getChild();
	Node* getSibling();
	Node* addSibling(Node* node);
private:
	Node* child;
	Node* sibling;
	Node* lastChild;
};

#endif