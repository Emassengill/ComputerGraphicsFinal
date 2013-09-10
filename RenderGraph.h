#ifndef __RENDER_GRAPH__
#define __RENDER_GRAPH__

#include "MatMath.h"

class Node;

class RenderGraph {
public:
	static const int MAX_LIGHTS = 1;
	//This array allows looping through the GL_TEXTURE constants by index.
	static const GLenum glTEXTURE[MAX_LIGHTS];

	RenderGraph(GLuint prog, Node& tree, const mat4& proj = MatMath::ID,
		const mat4& cam = MatMath::ID);
	~RenderGraph();
	void draw(const mat4& trans = MatMath::ID, const mat4& skew = MatMath::ID);
	void animate(float theta);
	void setProgram(GLuint prog);
	const mat4& getProjection() const;
	const mat4& getCamera() const;
	void setProjection(const mat4& proj);
	void setCamera(const mat4& cam);
	Node& root;
	GLuint program;
	GLuint camera_loc, proj_loc, shadow_loc;
	GLuint trans_loc, skew_loc;
	GLuint spec_loc, shadowSamp_loc;
	GLuint isSun_loc, isInside_loc, shadowMap_loc;
	GLuint shadowBuffer, shadowTexture[MAX_LIGHTS], shadowSampler[MAX_LIGHTS];
private:
	mat4 _projection;
	mat4 _camera;
};

#endif