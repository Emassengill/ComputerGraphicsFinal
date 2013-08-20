#ifndef __PRIMITIVE__
#define __PRIMITIVE__

#include "Definitions.h"
#include "Object.h"

class Primitive : public Object {
public:
	Primitive(GLuint vaobj, GLsizei numVerts, GLuint qry, GLfloat spec = 0.0);
	virtual ~Primitive() override;
	virtual void draw(const mat4& trans, const mat4& skew) override;
private:
	GLuint vao;
	GLsizei numVertices;
	GLuint occ;
	GLfloat specexp;
};

#endif