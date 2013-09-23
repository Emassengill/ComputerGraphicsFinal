#ifndef __RENDER_GRAPH__
#define __RENDER_GRAPH__

#include "MatMath.h"
#include "Global.h"

class EnvironmentNode;

class RenderGraph {
public:
	static const GLsizei MAX_LIGHTS = 1;

	RenderGraph(GLuint drawProg, GLuint shadowProg, EnvironmentNode& tree, const mat4& proj = MatMath::ID,
		const mat4& cam = MatMath::ID);
	~RenderGraph();
	void draw(const mat4& trans = MatMath::ID, const mat4& skew = MatMath::ID);
	void animate(float theta);
	void setProgram(GLuint drawProg, GLuint shadowProg);
	const mat4& getProjection() const;
	const mat4& getCamera() const;
	void setProjection(const mat4& proj);
	void setCamera(const mat4& cam);
	void drawMode(const mat4& shadowMat = MatMath::ID);
	void shadowMode(const mat4& projCam);
	void prepShadowMap(unsigned index = 0u) const;
	void bindShadowMap(unsigned index = 0u) const;
	EnvironmentNode& root;
	GLuint program;
	GLint trans_loc, skew_loc;
	GLint spec_loc;
	GLint isSun_loc, isInside_loc;
	GLint numLights_loc;
	bool shadowMapping;
private:
	mat4 _projection;
	mat4 _camera;

	GLuint _drawProgram;
	GLint _drawCam_loc, _drawProj_loc, _drawShadow_loc;
	GLint _drawTrans_loc, _drawSkew_loc;
	GLint _drawSpec_loc, _drawSun_loc, _drawInside_loc, _numLights_loc;
	GLint _shadowSamp_loc;

	GLuint _shadowProgram;
	GLint _shadowProjCam_loc;
	GLint _shadowTrans_loc;

	GLuint _shadowBuffer, _shadowTexture;
};

//PUBLIC

inline
const mat4& RenderGraph::getProjection() const { return _projection; }

inline
const mat4& RenderGraph::getCamera() const { return _camera; }

inline
void RenderGraph::setProjection(const mat4& proj) { _projection = proj; }

inline
void RenderGraph::setCamera(const mat4& cam) { _camera = cam; }

#endif