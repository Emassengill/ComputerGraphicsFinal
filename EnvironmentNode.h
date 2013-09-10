#ifndef __ENVIRONMENT_NODE__
#define __ENVIRONMENT_NODE__

#include "MatMath.h"
#include "LightNode.h"
#include "Container.h"
#include "BoolState.h"

class EnvironmentNode : public ObjectNode {
	//EnvironmentNodes specify self-contained lighting environments. They serve as gatekeepers to their
	//sub-trees during Global::lighting operations, such as getLight() and shadow mapping. This allows different
	//lighting environments to be children of another lighting environment without the parent's
	//lighting operations interfering with its children's. The house is a good example, with both
	//interior and exterior environments.
public:
	EnvironmentNode(BoolState& state = BoolState(), const mat4& trans = MatMath::ID,
		const mat4& skew = MatMath::ID, mat4 (*animFunc)(float) = nullptr);
	virtual ~EnvironmentNode() override;
	virtual void addChild(Node& node) override;
	virtual LightNode* getLight() override;
	virtual void draw(const RenderGraph& context, const mat4& trans, const mat4& skew, bool dynamic,
		bool drawMode) override;
private:
	void addLight(LightNode& light);
	Container<LightNode*> searchLights(Node& node);
	Container<LightNode*> _lights;
};

#endif