#include "LightNode.h"

//override
void LightNode::draw(RenderGraph& context, const mat4& trans, const mat4& skew,
	bool dynamic, bool drawing)
{
	ObjectNode::draw(context, trans, skew, dynamic, drawing);
	if (!cached()) _lastTrans = trans;
}

//override
GraphInfo LightNode::getInfo() const {
	return GraphInfo(const_cast<LightNode*>(this)).mergeWith(Node::getInfo());
}