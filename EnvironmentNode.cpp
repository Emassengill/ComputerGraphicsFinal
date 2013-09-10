#include "EnvironmentNode.h"
#include "LightNode.h"
#include "BoolState.h"
#include "RenderGraph.h"

#include <assert.h>
#include <vector>

EnvironmentNode::EnvironmentNode(BoolState& state, const mat4& trans, const mat4& skew,
	mat4 (*animFunc)(float)) : ObjectNode(new BoolState(state), trans, skew, animFunc)
{ }

//override
EnvironmentNode::~EnvironmentNode() { delete _object; }

//override
void EnvironmentNode::addChild(Node& node) {
	Container<LightNode*> newlights = searchLights(node);
	for (LightNode** light = newlights.get(), ** end = light + newlights.size(); light < end; ++light) {
		addLight(**light);
		(*light)->reset();
	}
	Node::addChild(node);
}

//override
LightNode* EnvironmentNode::getLight() { return nullptr; }

//override
void EnvironmentNode::draw(const RenderGraph& context, const mat4& trans, const mat4& skew,
	bool dynamic, bool drawMode)
{
#define __SHADOW_MAPPING_ON__
#ifdef __SHADOW_MAPPING_ON__

	//drawMode is true when another, higher EnvironmentNode is drawing its sub-trees.
	//When an EnvironmentNode in a sub-tree detects this, it blocks the draw() call
	//from drawing its own sub-trees.
	if (drawMode) return;

	if (!_lights.empty()) {
		//First, generate the shadow map
		glUniform1f(context.shadowMap_loc, 1.0f);

		const LightNode& light = *_lights.front();
		vec4 lightVec = light.genTranslation();
		mat4 lightView = LookAt(lightVec, vec3(0.0f), MatMath::yAXIS);
		mat4 lightProjection = light.genProjection();
		glUniformMatrix4fv(context.camera_loc, 1, GL_TRUE, lightView);
		glUniformMatrix4fv(context.proj_loc, 1, GL_TRUE, lightProjection);

		glBindFramebuffer(GL_FRAMEBUFFER, context.shadowBuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, context.shadowTexture[0], 0);
		glDrawBuffer(GL_NONE);
		glViewport(0, 0, Global::SHADOW_BUFFER_DIM, Global::SHADOW_BUFFER_DIM);
		glClearDepth(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(2.0f, 4.0f);

		ObjectNode::draw(context, trans, skew, dynamic, true); ////

		glDisable(GL_POLYGON_OFFSET_FILL);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(Global::viewx, Global::viewy, Global::viewDim, Global::viewDim);
		
		glUniformMatrix4fv(context.camera_loc, 1, GL_TRUE, context.getCamera());
		glUniformMatrix4fv(context.proj_loc, 1, GL_TRUE, context.getProjection());
		
		//Second, draw the subtrees
		glUniform1f(context.shadowMap_loc, 0.0f);

		mat4 scaleBias(	MatMath::translate(1.0f, 1.0f, 1.0f) * 0.5f );
		scaleBias[3][3] = 1.0f;
		glUniformMatrix4fv(context.shadow_loc, 1, GL_TRUE, scaleBias * lightProjection * lightView);
		for (int i = 0; i < context.MAX_LIGHTS; ++i) {
			glSamplerParameteri(context.shadowSampler[i], GL_TEXTURE_COMPARE_MODE,
				GL_COMPARE_REF_TO_TEXTURE);
			/*glActiveTexture(context.glTEXTURE[i]);
			glBindTexture(GL_TEXTURE_2D, context.shadowTexture[i]);*/
		}
	}
	ObjectNode::draw(context, trans, skew, dynamic, true);

	//Finally, traverse to any child EnvironmentNodes.
	ObjectNode::draw(context, trans, skew, dynamic, false);
#else
	ObjectNode::draw(context, trans, skew, dynamic, true);
#endif
}

//PRIVATE

void EnvironmentNode::addLight(LightNode& light) { _lights.push_back(&light); }

Container<LightNode*> EnvironmentNode::searchLights(Node& node) {
	Container<LightNode*> templist, results;
	for (LightNode* current = node.getLight(); current != nullptr; current = node.getLight()) {
		templist.push_back(current);
		while (!templist.empty()) {
			current = templist.back();
			assert(current != nullptr);
			current = current->getLight();
			if (current != nullptr) templist.push_back(current);
			else results.push_back(templist.pop_off());
		}
	}
	return results;
}