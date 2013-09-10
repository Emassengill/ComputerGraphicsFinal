#include "RenderGraph.h"
#include "Node.h"
#include "Global.h"

//PUBLIC

const GLenum RenderGraph::glTEXTURE[] = { GL_TEXTURE0 };

RenderGraph::RenderGraph(GLuint prog, Node& tree, const mat4& proj, const mat4& cam)
	: root(tree), _projection(proj), _camera(cam)
{
		setProgram(prog);
		glUseProgram(program);
		glGenFramebuffers(1, &shadowBuffer);
		glGenSamplers(MAX_LIGHTS, shadowSampler);
		glGenTextures(MAX_LIGHTS, shadowTexture);
		for (int i = 0; i < MAX_LIGHTS; ++i) {
			glBindTexture(GL_TEXTURE_2D, shadowTexture[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, Global::SHADOW_BUFFER_DIM,
				Global::SHADOW_BUFFER_DIM, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
}

RenderGraph::~RenderGraph() { delete &root; glDeleteProgram(program); }

void RenderGraph::draw(const mat4& trans, const mat4& skew) {
	glUseProgram(program);
	glUniformMatrix4fv(proj_loc, 1, GL_TRUE, _projection);
	glUniformMatrix4fv(camera_loc, 1, GL_TRUE, _camera);
	root.draw(*this, trans, skew, false, false);
}

void RenderGraph::animate(float theta) { root.animate(theta); }

void RenderGraph::setProgram(GLuint prog) {
	program = prog;
	camera_loc = glGetUniformLocation(program, "Camera");
	shadow_loc = glGetUniformLocation(program, "Shadows");
	proj_loc = glGetUniformLocation(program, "Projection");
	trans_loc = glGetUniformLocation(program, "vTrans");
	skew_loc = glGetUniformLocation(program, "nSkew");
	spec_loc = glGetUniformLocation(program, "specexp");
	shadowSamp_loc = glGetUniformLocation(program, "shadowSampler");
	isSun_loc = glGetUniformLocation(program, "isSun");
	isInside_loc = glGetUniformLocation(program, "isInside");
	shadowMap_loc = glGetUniformLocation(program, "shadowMap");

	glBindSampler(shadowSampler[0], shadowSamp_loc);
}

const mat4& RenderGraph::getProjection() const { return _projection; }
const mat4& RenderGraph::getCamera() const { return _camera; }
void RenderGraph::setProjection(const mat4& proj) { _projection = proj; }
void RenderGraph::setCamera(const mat4& cam) { _camera = cam; }