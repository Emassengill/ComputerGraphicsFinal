#include "RenderGraph.h"
#include "EnvironmentNode.h"

//PUBLIC

RenderGraph::RenderGraph(GLuint drawProg, GLuint shadowProg, EnvironmentNode& tree, const mat4& proj,
	const mat4& cam) : root(tree), _projection(proj), _camera(cam), shadowMapping(false)
{
	setProgram(drawProg, shadowProg);
	glUseProgram(drawProg);

	glGenFramebuffers(1, &_shadowBuffer);
	glGenTextures(1, &_shadowTexture);
	glBindTexture(GL_TEXTURE_2D, _shadowTexture);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Global::SHADOW_BUFFER_DIM,
					Global::SHADOW_BUFFER_DIM, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

RenderGraph::~RenderGraph() {
	delete &root;
	glDeleteProgram(_drawProgram);
	glDeleteProgram(_shadowProgram);
	glDeleteFramebuffers(1, &_shadowBuffer);
	glDeleteTextures(1, &_shadowTexture);
}

void RenderGraph::draw(const mat4& trans, const mat4& skew) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	drawMode();
	root.draw(*this, trans, skew, false, false);
}

void RenderGraph::animate(float theta) { root.animate(theta); }

void RenderGraph::setProgram(GLuint drawProg, GLuint shadowProg) {
	_drawProgram = drawProg;
	_drawCam_loc = glGetUniformLocation(_drawProgram, "camera");
	_drawProj_loc = glGetUniformLocation(_drawProgram, "projection");
	_drawShadow_loc = glGetUniformLocation(_drawProgram, "shadowMatrix");
	_drawTrans_loc = glGetUniformLocation(_drawProgram, "vTrans");
	_drawSkew_loc = glGetUniformLocation(_drawProgram, "nSkew");
	_drawSpec_loc = glGetUniformLocation(_drawProgram, "specexp");
	_drawSun_loc = glGetUniformLocation(_drawProgram, "isSun");
	_drawInside_loc = glGetUniformLocation(_drawProgram, "isInside");
	_numLights_loc = glGetUniformLocation(_drawProgram, "numLights");
	_shadowSamp_loc = glGetUniformLocation(_drawProgram, "shadowSampler");

	_shadowProgram = shadowProg;
	_shadowProjCam_loc = glGetUniformLocation(_shadowProgram, "proj_cam");
	_shadowTrans_loc = glGetUniformLocation(_shadowProgram, "vTrans");
}

void RenderGraph::drawMode(const mat4& shadowMat) {
	shadowMapping = false;
	program = _drawProgram;
	glUseProgram(program);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);
	glDrawBuffer(GL_BACK);

	glViewport(Global::viewx, Global::viewy, Global::viewDim, Global::viewDim);
	glDisable(GL_POLYGON_OFFSET_FILL);

	trans_loc = _drawTrans_loc;
	skew_loc =_drawSkew_loc;
	isSun_loc = _drawSun_loc;
	isInside_loc = _drawInside_loc;
	spec_loc = _drawSpec_loc;

	glUniformMatrix4fv(_drawCam_loc, 1, GL_TRUE, _camera);
	glUniformMatrix4fv(_drawProj_loc, 1, GL_TRUE, _projection);
	glUniformMatrix4fv(_drawShadow_loc, 1, GL_TRUE, shadowMat);
	glUniform1i(_shadowSamp_loc, GL_TEXTURE0);
}

void RenderGraph::shadowMode(const mat4& projCam) {
	shadowMapping = true;
	program = _shadowProgram;
	glUseProgram(program);

	glBindFramebuffer(GL_FRAMEBUFFER, _shadowBuffer);
	glCullFace(GL_FRONT);
	glDrawBuffer(GL_NONE);

	glViewport(0, 0, Global::SHADOW_BUFFER_DIM, Global::SHADOW_BUFFER_DIM);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 4.0f);

	trans_loc = _shadowTrans_loc;
	skew_loc = isSun_loc = isInside_loc = spec_loc = _drawSpec_loc = -1;

	glUniformMatrix4fv(_shadowProjCam_loc, 1, GL_TRUE, projCam);
}

void RenderGraph::prepShadowMap(unsigned index) const {
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _shadowTexture, 0);
}

void RenderGraph::bindShadowMap(unsigned index) const {
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, _shadowTexture);
}