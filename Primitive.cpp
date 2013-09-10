#include "Primitive.h"
#include "RenderGraph.h"

//PUBLIC

Primitive::Primitive(GLuint vaobj, GLsizei numVerts, GLuint qry, GLfloat spec)
	: _vao(vaobj), _numVertices(numVerts), _occ(qry), _specexp(spec) { }

//override
Primitive::~Primitive() {
	glDeleteVertexArrays(1, &_vao);
	if (_occ != (GLuint)0) glDeleteQueries(1, &_occ);
}

//override
void Primitive::draw(const RenderGraph& context, const mat4& trans, const mat4& skew) const {
	glUniformMatrix4fv(context.trans_loc, 1, GL_TRUE, trans);
	glUniformMatrix4fv(context.skew_loc, 1, GL_TRUE, skew);
	glUniform1f(context.spec_loc, _specexp);
	
	glBindVertexArray(_vao);
	glBeginConditionalRender(_occ, GL_QUERY_NO_WAIT);
	glDrawElements(GL_TRIANGLES, _numVertices, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	glEndConditionalRender();
	glBindVertexArray(0);
}