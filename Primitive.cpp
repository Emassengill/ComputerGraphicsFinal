#include "Primitive.h"
#include "Global.h"

//PUBLIC

Primitive::Primitive(GLuint vaobj, GLsizei numVerts, GLuint qry, GLfloat spec)
	: vao(vaobj), numVertices(numVerts), occ(qry), specexp(spec) { }

//override
Primitive::~Primitive() {
	glDeleteVertexArrays(1, &vao);
	if (occ != (GLuint)0) glDeleteQueries(1, &occ);
}

//override
void Primitive::draw(const mat4& trans, const mat4& skew) {
	glUniformMatrix4fv(Global::trans_loc, 1, GL_TRUE, trans);
	glUniformMatrix4fv(Global::skew_loc, 1, GL_TRUE, skew);
	glUniform1f(Global::spec_loc, specexp);
	
	glBindVertexArray(vao);
	glBeginConditionalRender(occ, GL_QUERY_NO_WAIT);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	glEndConditionalRender();
	glBindVertexArray(0);
}