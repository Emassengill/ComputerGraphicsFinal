#ifndef __PRIMITIVE__
#define __PRIMITIVE__

#include "Definitions.h"
#include "Object.h"

class Primitive : public Object {
public:
	Primitive(GLuint vaobj, GLsizei numVerts, GLuint qry, GLfloat spec = 0.0);
	virtual ~Primitive() override;
	virtual void draw(const RenderGraph& context, const mat4& trans, const mat4& skew) const override;
private:
	GLuint _vao;
	GLsizei _numVertices;
	GLuint _occ;
	GLfloat _specexp;
};

#endif