#ifndef __ENVIRONMENT_NODE__
#define __ENVIRONMENT_NODE__

#include "MatMath.h"
#include "ObjectNode.h"
#include "Container.h"
#include "BoolState.h"

#include <assert.h>

class LightNode;
class RenderGraph;

class EnvironmentNode : public ObjectNode {
/*EnvironmentNodes specify self-contained lighting environments. They serve as gatekeepers to their
**sub-trees during Global::lighting operations, such as getLight() and shadow mapping. This allows different
**lighting environments to be children of another lighting environment without the parent's
**lighting operations interfering with its children's. The house is a good example, with both
**interior and exterior environments.
*/
public:
	static const mat4 scaleBias;
	EnvironmentNode(BoolState& state = BoolState(), const mat4& trans = MatMath::ID,
		const mat4& skew = MatMath::ID, mat4 (*animFunc)(float) = nullptr);
	virtual ~EnvironmentNode() override;
	virtual void addChild(Node& node) override;
	virtual GraphInfo getInfo() const override;
	virtual void draw(RenderGraph& context, const mat4& trans, const mat4& skew, bool dynamic,
		bool drawMode) override;
private:
	Container<LightNode*> _lights;
	bool _hasEnvironmentChild;
};

//PUBLIC

inline
EnvironmentNode::EnvironmentNode(BoolState& state, const mat4& trans, const mat4& skew,
	mat4 (*animFunc)(float))
	: ObjectNode(new BoolState(state), trans, skew, animFunc), _hasEnvironmentChild(false)
{ }

#endif