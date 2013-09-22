#include "ObjectNode.h"

//override
void ObjectNode::transform(const mat4& trans, const mat4& skew) {
	_localTrans = trans * _localTrans;
	_localSkew = skew * _localSkew;
}

//override
void ObjectNode::draw(RenderGraph& context, const mat4& trans, const mat4& skew, bool dynamic,
	bool drawing)
//_cached flags whether the node has stored the result of its matrix multiplication.
//On the first call of draw(), if a node is not animated, it stores the result of its
//local multiplication. On future calls, instead of recalculating the same product, it
//uses the stored result. If the node is animated, or the child of an animated node, then
//it continues to perform multiplications every frame, ensuring that nodes participating
//in an _animation do not become frozen.
//drawing flags that the enclosing environment is actively drawing the subtree. If it is false,
//the subtree is merely traversed until the next EnvironmentNode. This ensures that the drawing of
//child environments is encapsulated from the drawing of their parents.
{
	if (!dynamic && _animation == nullptr) { //if neither this or a parent is animated
		if (!_cached) {//cache matrices if not _cached
			_cached = true;
			_localTrans = trans * _localTrans;
			_localSkew = skew * _localSkew;
		}
		if (drawing && _object != nullptr) _object->draw(context, _localTrans, _localSkew);
		Node::draw(context, _localTrans, _localSkew, false, drawing);
	} else { //if this or a parent is animated
		const mat4 drawTrans = trans * _localTrans;
		const mat4 drawSkew = skew * _localSkew;
		if (drawing && _object != nullptr) _object->draw(context, drawTrans, drawSkew);
		Node::draw(context, drawTrans, drawSkew, true, drawing);
	}
}

//override
void ObjectNode::animate(float number) {
	if (_animation != nullptr) {
		const mat4 animTrans = _animation(number);
		transform(animTrans, animTrans);
	}
	Node::animate(number);
}