#include "EnvironmentNode.h"
#include "LightNode.h"
#include "RenderGraph.h"

//PUBLIC

const mat4 EnvironmentNode::scaleBias(	0.5f, 0.0f, 0.0f, 0.0f,
										0.0f, 0.5f, 0.0f, 0.0f,
										0.0f, 0.0f, 0.5f, 0.0f,
										0.5f, 0.5f, 0.5f, 1.0f	);

//override
EnvironmentNode::~EnvironmentNode() { delete _object; }

//override
void EnvironmentNode::addChild(Node& node) {
	GraphInfo info = node.getInfo();
	_lights.push_block(info.lights);
	_hasEnvironmentChild = _hasEnvironmentChild || info.hasEnvironment;
	Node::addChild(node);
}

//override
GraphInfo EnvironmentNode::getInfo() const { return GraphInfo(true); }

//override
void EnvironmentNode::draw(RenderGraph& context, const mat4& trans, const mat4& skew,
	bool dynamic, bool drawing)
{
	//drawing is true when another, higher EnvironmentNode is drawing its sub-trees.
	//When an EnvironmentNode in a sub-tree detects this, it blocks the draw() call
	//from drawing its own sub-trees.
	if (drawing) return;

	if (!_lights.empty()) {
		const unsigned numLights = _lights.size();
		glUniform1ui(context.numLights_loc, numLights);
		const LightNode& light = *( _lights.front() );
		const mat4 lightProjCam = light.genProjection() * light.genCamera(context.getCamera());

		//First, generate the shadow maps
		context.shadowMode(lightProjCam);
		context.prepShadowMap();
		
		switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
		case GL_FRAMEBUFFER_COMPLETE: break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cout << "Framebuffer: Incomplete attachment.\n"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cout << "Framebuffer: Incomplete missing attachment.\n"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			std::cout << "Framebuffer: Incomplete dimensions.\n"; break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cout << "Framebuffer: Framebuffer unsupported.\n";
		}

		ObjectNode::draw(context, trans, skew, dynamic, true); ////
		
		//Second, draw the subtrees
		context.drawMode(scaleBias * lightProjCam);
		context.bindShadowMap();
	}

	ObjectNode::draw(context, trans, skew, dynamic, true);

	//Finally, traverse to any child EnvironmentNodes, if there are any.
	if (_hasEnvironmentChild) ObjectNode::draw(context, trans, skew, dynamic, false);
}