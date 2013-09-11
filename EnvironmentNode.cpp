#include "EnvironmentNode.h"
#include "LightNode.h"
#include "BoolState.h"
#include "RenderGraph.h"

#include <assert.h>

//PUBLIC

const mat4 EnvironmentNode::scaleBias(	0.5f, 0.0f, 0.0f, 0.0f,
										0.0f, 0.5f, 0.0f, 0.0f,
										0.0f, 0.0f, 0.5f, 0.0f,
										0.5f, 0.5f, 0.5f, 1.0f		);

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
		unsigned numLights = _lights.size();
		glUniform1i(context.numLights_loc, numLights);

		//First, generate the shadow maps
		glUniform1f(context.shadowMap_loc, 1.0f);

		mat4* lightViews = new mat4[numLights];
		mat4* lightProjections = new mat4[numLights];

		for (int i = 0; i < numLights; ++i) {
			const LightNode& light = *_lights.front();
			const vec4 lightVec = light.genTranslation();
			lightViews[i] = LookAt(lightVec, vec3(0.0f), MatMath::yAXIS);
			lightProjections[i] = light.genProjection();
			glUniformMatrix4fv(context.camera_loc, 1, GL_TRUE, lightViews[i]);
			glUniformMatrix4fv(context.projection_loc, 1, GL_TRUE, lightProjections[i]);

			glBindFramebuffer(GL_FRAMEBUFFER, context.shadowBuffer);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, context.shadowTextures[i], 0);
			glDrawBuffer(GL_NONE);
			glViewport(0, 0, Global::SHADOW_BUFFER_DIM, Global::SHADOW_BUFFER_DIM);
			glClearDepth(1.0f);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(2.0f, 4.0f);

			ObjectNode::draw(context, trans, skew, dynamic, true); ////
		}

		glDisable(GL_POLYGON_OFFSET_FILL);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(Global::viewx, Global::viewy, Global::viewDim, Global::viewDim);
		
		glUniformMatrix4fv(context.camera_loc, 1, GL_TRUE, context.getCamera());
		glUniformMatrix4fv(context.projection_loc, 1, GL_TRUE, context.getProjection());
		
		//Second, draw the subtrees
		glUniform1f(context.shadowMap_loc, 0.0f);

		GLfloat* const shadowMatrices = new GLfloat[numLights * 16];
		for (int i = 0; i < numLights; ++i) {
			glActiveTexture(RenderGraph::glTEXTURE[i]);
			glBindTexture(GL_TEXTURE_2D, context.shadowTextures[i]);
			mat4 result = scaleBias * lightProjections[i] * lightViews[i];
			memcpy(shadowMatrices + i * sizeof(mat4), static_cast<const GLfloat*>(result), sizeof(mat4));
		}
		delete[] lightProjections; delete[] lightViews;

		glUniformMatrix4fv(context.shadowers_loc, numLights, GL_TRUE, shadowMatrices);

		delete[] shadowMatrices;
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