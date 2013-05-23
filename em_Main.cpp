#include "Angel.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

//Global Constants and Variables

typedef vec4 color4;
typedef vec4 point4;
typedef vec4 normal4;

const mat4 ID = mat4(1.0);
const point4 ORIGIN = point4(0.0, 0.0, 0.0, 1.0);

const normal4 xNorm = normal4(1.0, 0.0, 0.0, 0.0);
const normal4 yNorm = normal4(0.0, 1.0, 0.0, 0.0);
const normal4 zNorm = normal4(0.0, 0.0, 1.0, 0.0);
const vec3 axes[3] = {
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
};

// RGBA color palette
const color4 white_opaque = color4( 1.0, 1.0, 1.0, 1.0 );
const color4 black_opaque = color4( 0.0, 0.0, 0.0, 1.0 );
const color4 steel_opaque = color4( 0.75, 0.75, 0.75, 1.0 );
const color4 red_opaque = color4( 1.0, 0.0, 0.0, 1.0 );
const color4 darkred_opaque = color4( 0.7, 0.1, 0.0, 1.0 );
const color4 darkgreen_opaque = color4( 0.2, 0.65, 0.2, 1.0 );
const color4 blue_opaque = color4( 0.0, 0.0, 1.0, 1.0 );
const color4 yellow_opaque = color4( 1.0, 1.0, 0.0, 1.0 );
const color4 darkbrown_opaque = color4( 0.6, 0.4, 0.2, 1.0 );

const float speed = 0.5;
bool crouching = false;
bool animating = true;

const float zoomin = 1.2;
const float zoomout = 1.0/zoomin;

const float FOV = 85.0;
const mat4 projMat = Perspective(FOV, 1.0, 0.01, 40.0);
vec4 sunVec = vec4(0.0, 1.0, 0.0, 0.0);

//shader pointers
GLuint currentProg;
GLuint camera_loc, trans_loc, skew_loc, sun_loc, isSun_loc, isInside_loc;

//Node and Object definitions
class Object {
protected:
	virtual void inheritDraw(const mat4& trans, const mat4& skew) = 0;
public:
	virtual void draw(const mat4& trans = ID, const mat4& skew = ID) { inheritDraw(trans, skew); }
};

class Primitive : public Object {
private:
	GLuint vao;
	GLsizei numVertices;
protected:
	virtual void inheritDraw(const mat4& trans, const mat4& skew) override {
		glUniformMatrix4fv(trans_loc, 1, GL_TRUE, trans);
		glUniformMatrix4fv(skew_loc, 1, GL_TRUE, skew);
	
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
		glBindVertexArray(0);
	}
public:
	Primitive(const GLuint vaobj, const GLint numVerts) : vao(vaobj), numVertices(numVerts) {}
	~Primitive() { glDeleteVertexArrays(1, &vao); }
};
class PointLight : public Object {
private:
	char* target;
protected:
	virtual void inheritDraw(const mat4& trans, const mat4& skew) override {
		GLuint light_loc = glGetUniformLocation(currentProg, target);
		glUniform4fv(light_loc, 1, vec4(trans[0][3], trans[1][3], trans[2][3], trans[3][3]));
	}
public:
	PointLight(char* const targ) : target(targ) {}
};
class BoolState : public Object {
private:
	float isInside;
	float isSun;
protected:
	virtual void inheritDraw(const mat4& trans, const mat4& skew) override {
		glUniform1f(isInside_loc, isInside);
		glUniform1f(isSun_loc, isSun);
	}
public:
	BoolState(const float inside = 0.0, const float sun = 0.0) : isInside(inside), isSun(sun) {}
};

class Node {
private:
	Object* object;
	Node* child;
	Node* sibling;
	mat4 localTrans;
	mat4 localSkew;
	mat4 (*animation)(const float);
public:
	Node(Object* obj = nullptr, Node* sub = nullptr, Node* next = nullptr,
		const mat4& trans = ID, const mat4& skew = ID, mat4 (*animFunc)(const float) = nullptr) :
		object(obj), child(sub), sibling(next), localTrans(trans), localSkew(skew), animation(animFunc) {}
	~Node() {
		if (child != nullptr) delete child;
		if (sibling != nullptr) delete sibling;
		if (object != nullptr) delete object;
	}

	void transform(const mat4& trans, const mat4& skew = ID) {
		localTrans = trans * localTrans;
		localSkew = skew * localSkew;
	}
	void draw(const mat4& trans = ID, const mat4& skew = ID) {
		const mat4 drawTrans = trans * localTrans;
		const mat4 drawSkew = skew * localSkew;
		if (object != nullptr) object->draw(drawTrans, drawSkew);
		if (child != nullptr) child->draw(drawTrans, drawSkew);
		if (sibling != nullptr) sibling->draw(trans, skew);
	}
	void animate(const float number = 0.0, const bool parentCall = true) {
		if (animation != nullptr) {
			const mat4 animTrans = animation(number);
			localTrans = animTrans * localTrans;
			localSkew = animTrans * localSkew;
		}
		if (child != nullptr) child->animate(number);
		if (parentCall && sibling != nullptr) sibling->animate(number);	
	}
	void addChild(Node* node) {
		if (child == nullptr) child = node;
		else for (Node* current = child; current != nullptr; current = current->sibling)
			if (current->sibling == nullptr) {
				current->sibling = node;
				break;
			}
	}
};

//Global Objects
//For Chassis
Object *carSphere, *carCube;
//For Wheel
Object *wheelTube, *wheelCylinder;
//For Road
Object *roadRing;
//For Ground
Object *groundSquare;
//For Leaf
Object *leaf[2];
//For Tree
Object *treeCone, *treeCylinder;
//For Sun
Object *sunDisk;
//For House
Object *houseCube, *housePyramid, *houseSquare;
//For Furniture
Object *furnTube, *furnCube0, *furnCube1;
//For Special
Object *light, *sunState, *insideState, *outsideState;

//tree
Node* root = nullptr;
Node* sun = nullptr;

//camera matrices
mat4 positMat = ID;
mat4 pitchMat = ID;
mat4 rollMat = ID;
mat4 yawMat = ID;

float pitchTheta = 0.0;

//window width and height
int ww = 512;
int wh = 512;
//viewport (x, y) coordinate, and viewport width-height (same size)
int viewx = 0;
int viewy = 0;
int viewDim = 512;

//mouse callback values
bool leftMouse = false;
int pRef = 0, qRef = 0;

//coordinate system conversion functions
inline int coord2screen(const float x) {
	return (int) ((x + 1.0)/2.0) * viewDim;
}
inline float screen2coord(const int x) {
	return  2.0 * x / viewDim - 1.0;
}

inline int round(const float x) { return (int)floor(x + 0.5); }
inline int getMin(const float first, const float second, const float third) {
	if (first <= second) {
		if (first <= third) return 0;
		else return 2;
	} else /*second < first*/ {
		if (second <= third) return 1;
		else return 2;
	}
}
inline int getMax(const float first, const float second, const float third) {
	if (first >= second) {
		if (first >= third) return 0;
		else return 2;
	} else /*second > first*/ {
		if (second >= third) return 1;
		else return 2;
	}
}
inline float randNum(const float start, const float end) {
	return (end - start) * rand() / RAND_MAX + start;
}
inline float bernoulli() { return (float) rand() / RAND_MAX; }

//rotation matrix generators
inline mat4 rX(const float angle) {
	const float cosine = cos(angle);
	const float sine = sin(angle);
	return mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, cosine, sine, 0.0,
		0.0, -sine, cosine, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}
inline mat4 rY(const float angle) {
	const float cosine = cos(angle);
	const float sine = sin(angle);
	return mat4(
		cosine, 0.0, -sine, 0.0,
		0.0, 1.0, 0.0, 0.0,
		sine, 0.0, cosine, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}
inline mat4 rZ(const float angle) {
	const float cosine = cos(angle);
	const float sine = sin(angle);
	return mat4(
		cosine, sine, 0.0, 0.0,
		-sine, cosine, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}
//rotation matrix for an arbitrary axis
mat4 rO(const float theta, const float dx, const float dy, const float dz) {
	const vec3 v3 = normalize( vec3(dx, dy, dz) );
	
	const int chosenAxis = getMin(
		abs(dot(v3, axes[0])),
		abs(dot(v3, axes[1])),
		abs(dot(v3, axes[2]))
		);

	const vec3 v2 = normalize( cross(v3, axes[chosenAxis]) );
	const vec3 v1 = normalize( cross(v2, v3) );
	const mat3 newBasis = mat3(v1, v2, v3);

	const float cst = cos(theta);
	const float snt = sin(theta);
	const mat3 rotate = mat3(
		cst, snt, 0.0,
		-snt, cst, 0.0,
		0.0, 0.0, 1.0
		);

	const mat3 rotated = transpose(newBasis) * rotate * newBasis;
	
	return mat4(
		rotated[0][0], rotated[1][0], rotated[2][0], 0.0,
		rotated[0][1], rotated[1][1], rotated[2][1], 0.0,
		rotated[0][2], rotated[1][2], rotated[2][2], 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}
mat4 rO(const float theta, const vec3& axis) {
	const vec3 v3 = normalize(axis);
	
	const int chosenAxis = getMin(
		abs(dot(v3, axes[0])),
		abs(dot(v3, axes[1])),
		abs(dot(v3, axes[2]))
		);

	const vec3 v2 = normalize( cross(v3, axes[chosenAxis]) );
	const vec3 v1 = normalize( cross(v2, v3) );
	const mat3 newBasis = mat3(v1, v2, v3);

	const float cst = cos(theta);
	const float snt = sin(theta);
	const mat3 rotate = mat3(
		cst, snt, 0.0,
		-snt, cst, 0.0,
		0.0, 0.0, 1.0
		);

	const mat3 rotated = transpose(newBasis) * rotate * newBasis;
	
	return mat4(
		rotated[0][0], rotated[1][0], rotated[2][0], 0.0,
		rotated[0][1], rotated[1][1], rotated[2][1], 0.0,
		rotated[0][2], rotated[1][2], rotated[2][2], 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}

//transformation matrix generators
inline mat4 scale(const float factor) {
	return mat4(
		factor, 0.0, 0.0, 0.0,
		0.0, factor, 0.0, 0.0,
		0.0, 0.0, factor, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}
inline mat4 translate(const float x, const float y, const float z) {
	return mat4(	1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					x, y, z, 1.0		);
}
inline mat4 distort(const float x, const float y, const float z) {
	return mat4(	x, 0.0, 0.0, 0.0,
					0.0, y, 0.0, 0.0,
					0.0, 0.0, z, 0.0,
					0.0, 0.0, 0.0, 1.0	);
}

//Mesh Generators

struct SquareGen {
	//four points, four colors, two normals
	//two sides, two triangles per side, three vertices per triangle, 3 attributes per vertex
	static const int numElements = 4 + 4 + 2;
	static const int numVertices = 4 + 4;
	static const int numIndices = 2 * 2 * 3;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const GLsizei sizeData = 3 * numVertices * sizeof(vec4);
	static const GLsizei sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = 2 * numVertices * sizeof(vec4);

	void generate(	const color4& color0, const color4& color1,
					const color4& color2, const color4& color3	) {
		//set vertices
		elements[0] = point4(-1.0, -1.0, 0.0, 1.0); elements[1] = point4(1.0, -1.0, 0.0, 1.0);
		elements[2] = point4(1.0, 1.0, 0.0, 1.0); elements[3] = point4(-1.0, 1.0, 0.0, 1.0);
		//set colors
		elements[4] = color0; elements[5] = color1; elements[6] = color2; elements[7] = color3;
		//set normals
		elements[8] = zNorm; elements[9] = -zNorm;

		//front square
		data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2]; data[3] = elements[3];
		//back square
		data[4] = elements[3]; data[5] = elements[2]; data[6] = elements[1]; data[7] = elements[0];
		//front colors
		data[8] = elements[4]; data[9] = elements[5]; data[10] = elements[6]; data[11] = elements[7];
		//back colors
		data[12] = elements[7]; data[13] = elements[6]; data[14] = elements[5]; data[15] = elements[4];
		//front normals
		data[16] = data[17] = data[18] = data[19] = elements[8];
		//back normals
		data[20] = data[21] = data[22] = data[23] = elements[9];

		//front triangles
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 0; indices[4] = 2; indices[5] = 3;
		//back triangles
		indices[6] = 4; indices[7] = 5; indices[8] = 6;
		indices[9] = 4; indices[10] = 6; indices[11] = 7;
	}

	SquareGen(	const color4& color_1, const color4& color_2,
				const color4& color_3, const color4& color_4	) {
		generate(color_1, color_2, color_3, color_4);
	}
	SquareGen(const color4& color) { generate(color, color, color, color); }
	//SquareGen() { generate(black_opaque, black_opaque, black_opaque, black_opaque); }
};
struct TriangleGen {
	//3 points, 3 colors, 2 normals
	//2 sides, 3 vertices per side, 3 attributes per vertex
	static const int numElements = 3 + 3 + 2;
	static const int numVertices = 3 + 3;
	static const int numIndices = 2 * 3;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const int sizeData = 3 * numVertices * sizeof(vec4);
	static const int sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = numVertices * 2 * sizeof(vec4);

	TriangleGen(const color4& color0, const color4& color1, const color4& color2) {
		//set vertices
		elements[0] = point4(-sqrt(3.0)/2.0, -0.5, 0.0, 1.0);
		elements[1] = point4(sqrt(3.0)/2.0, -0.5, 0.0, 1.0);
		elements[2] = point4(0.0, 1.0, 0.0, 1.0);
		//set colors
		elements[3] = color0; elements[4] = color1; elements[5] = color2;
		//set normals
		elements[6] = zNorm; elements[7] = -zNorm;

		//front triangle
		data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2];
		//back triangle
		data[3] = elements[2]; data[4] = elements[1]; data[5] = elements[0];
		//front colors
		data[6] = elements[3]; data[7] = elements[4]; data[8] = elements[5];
		//back colors
		data[9] = elements[5]; data[10] = elements[4]; data[11] = elements[3];
		//front normals
		data[12] = data[13] = data[14] = elements[6];
		//back normals
		data[15] = data[16] = data[17] = elements[7];

		//front indices
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		//back indices
		indices[3] = 3; indices[4] = 4; indices[5] = 5;
	}
	TriangleGen(const color4& color) { TriangleGen(color, color, color); }
};
struct CircleGen {
	//unknown number of vertices
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(const int numEdgePoints, const color4& insideColor, const color4& outsideColor) {
		const int numPoints = numEdgePoints + 1;
		const float theta = 2.0 * M_PI / numEdgePoints;

		//numPoints vertices, 2 colors, 2 normals
		//2 sides, numEdgePoints triangles per side, 3 vertices per triangle, 3 attribute per vertex
		numElements = numPoints + 2 + 2;
		numVertices = 2 * numPoints;
		numIndices = 2 * numEdgePoints * 3;

		elements = new vec4[numElements];
		data = new vec4[3 * numVertices];
		indices = new GLubyte[numIndices];

		sizeData = 3 * numVertices * sizeof(vec4);
		sizeIndices = numIndices * sizeof(GLubyte);

		colors = numVertices * sizeof(vec4);
		normals = numVertices * 2 * sizeof(vec4);

		//set vertices
		elements[0] = ORIGIN;
		for (int i = 0; i < numEdgePoints; ++i)
			elements[i + 1] = point4(cos(i * theta), sin(i * theta), 0.0, 1.0);
		//set colors
		elements[numPoints] = insideColor;
		elements[numPoints + 1] = outsideColor;
		//set normals
		elements[numPoints + 2] = zNorm; elements[numPoints + 3] = -zNorm;

		//front circle
		data[0] = elements[0];
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + 1] = elements[i + 1];
		//back circle
		data[numPoints] = elements[0];
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + numPoints + 1] = elements[numEdgePoints - i];
		//front colors
		data[2 * numPoints] = insideColor;
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + 2 * numPoints + 1] = outsideColor;
		//back colors
		data[3 * numPoints] = insideColor;
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + 3 * numPoints + 1] = outsideColor;
		//front normals
		for (int i = 0; i < numPoints; ++i)
			data[i + 4 * numPoints] = zNorm;
		//back normals
		for (int i = 0; i < numPoints; ++i)
			data[i + 5 * numPoints] = -zNorm;


		//front indices
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			indices[i * 3] = 0; indices[i * 3 + 1] = i + 1; indices[i * 3 + 2] = i + 2;
		}
		indices[(numEdgePoints - 1) * 3] = 0;
		indices[(numEdgePoints - 1) * 3 + 1] = numEdgePoints;
		indices[(numEdgePoints - 1) * 3 + 2] = 1;
		//back indices
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			indices[3 * (i + numEdgePoints)] = numPoints;
			indices[3 * (i + numEdgePoints) + 1] = i + 1 + numPoints;
			indices[3 * (i + numEdgePoints) + 2] = i + 2 + numPoints;
		}
		indices[(2 * numEdgePoints - 1) * 3] = numPoints;
		indices[(2 * numEdgePoints - 1) * 3 + 1] = numEdgePoints + numPoints;
		indices[(2 * numEdgePoints - 1) * 3 + 2] = 1 + numPoints;
	}
	CircleGen(const int numEdgePoints, const color4& color0, const color4& color1) {
		generate(numEdgePoints, color0, color1);
	}
	CircleGen(const int numEdgePoints, const color4& color) { generate(numEdgePoints, color, color); }
	~CircleGen() { delete[] elements; delete[] data; delete[] indices; }
};
struct RingGen {
	//unknown number of vertices
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate (const float holeR, const int numEdgePoints,
					const color4& insideColor, const color4& outsideColor ) {
		if (holeR > 1.0 || holeR < 0.0) throw 11;

		const float theta = 2.0 * M_PI / numEdgePoints;

		//2 * numEdgePoints points, 2 colors, 2 normals
		//2 sides, numEdgePoints quadrilaterals per side, 2 triangles per quadrilateral,
		//3 vertices per triangle, 3 attributes per vertex
		numElements = 2 * numEdgePoints + 2 + 2;
		numVertices = 2 * numEdgePoints * 2;
		numIndices = 2 * numEdgePoints * 2 * 3;

		elements = new vec4[numElements];
		data = new vec4[3 * numVertices];
		indices = new GLubyte[numIndices];

		sizeData = 3 * numVertices * sizeof(vec4);
		sizeIndices = numIndices * sizeof(GLubyte);

		colors = numVertices * sizeof(vec4);
		normals = numVertices * 2 * sizeof(vec4);

		//set vertices
		//inside
		for (int i = 0; i < numEdgePoints; ++i) {
			//inside
			elements[i] = point4(holeR * cos(i * theta), holeR * sin(i * theta), 0.0, 1.0);
			//outside
			elements[i + numEdgePoints] = point4(cos(i * theta), sin(i * theta), 0.0, 1.0);
		}
		//set colors
		int temp = 2 * numEdgePoints;
		elements[temp] = insideColor; elements[temp + 1] = outsideColor;
		//set normals
		elements[temp + 2] = zNorm; elements[temp + 3] = -zNorm;

		//points
		for (int i = 0; i < numEdgePoints; ++i) {
			//front
			//inside
			data[i] = elements[i];
			//outside
			data[i + numEdgePoints] = elements[i + numEdgePoints];
			//back
			//inside
			data[i + 2 * numEdgePoints] = elements[numEdgePoints - 1 - i];
			//outside
			data[i + 3 * numEdgePoints] = elements[2 * numEdgePoints - 1 - i];
		}
		//colors
		for (int i = 0; i < numEdgePoints; ++i) {
			//front
			//inside
			data[i + 4 * numEdgePoints] = insideColor;
			//outside
			data[i + 5 * numEdgePoints] = outsideColor;
			//back
			//inside
			data[i + 6 * numEdgePoints] = insideColor;
			//outside
			data[i + 7 * numEdgePoints] = outsideColor;
		}
		//normals
		for (int i = 0; i < 2 * numEdgePoints; ++i) {
			//front
			data[i + 8 * numEdgePoints] = zNorm;
			//back
			data[i + 10 * numEdgePoints] = -zNorm;
		}
		
		//front ring
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			//first triangle
			indices[i * 6] = i + 1;
			indices[i * 6 + 1] = i;
			indices[i * 6 + 2] = i + numEdgePoints;
			//second triangle
			indices[i * 6 + 3] = i + numEdgePoints;
			indices[i * 6 + 4] = i + 1 + numEdgePoints;
			indices[i * 6 + 5] = i + 1;
		}
		//first triangle
		indices[6 * (numEdgePoints - 1)] = 0;
		indices[6 * (numEdgePoints - 1) + 1] = numEdgePoints - 1;
		indices[6 * (numEdgePoints - 1) + 2] = 2 * numEdgePoints - 1;
		//second triangle
		indices[6 * (numEdgePoints - 1) + 3] = 2 * numEdgePoints - 1;
		indices[6 * (numEdgePoints - 1) + 4] = numEdgePoints;
		indices[6 * (numEdgePoints - 1) + 5] = 0;
		//back ring
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			//first triangle
			indices[(i + numEdgePoints) * 6] = i + 1 + 2 * numEdgePoints;
			indices[(i + numEdgePoints) * 6 + 1] = i + 2 * numEdgePoints;
			indices[(i + numEdgePoints) * 6 + 2] = i + numEdgePoints + 2 * numEdgePoints;
			//second triangle
			indices[(i + numEdgePoints) * 6 + 3] = i + numEdgePoints + 2 * numEdgePoints;
			indices[(i + numEdgePoints) * 6 + 4] = i + 1 + numEdgePoints + 2 * numEdgePoints;
			indices[(i + numEdgePoints) * 6 + 5] = i + 1 + 2 * numEdgePoints;
		}
		//first triangle
		indices[12 * numEdgePoints - 6] =  2 * numEdgePoints;
		indices[12 * numEdgePoints - 5] = 3 * numEdgePoints - 1;
		indices[12 * numEdgePoints - 4] = 4 * numEdgePoints - 1;
		//second triangle
		indices[12 * numEdgePoints - 3] = 4 * numEdgePoints - 1;
		indices[12 * numEdgePoints - 2] = 3 * numEdgePoints;
		indices[12 * numEdgePoints - 1] = 2 * numEdgePoints;
	}
	RingGen(const float holeR, const int numEdgePoints,
					const color4& insideColor, const color4& outsideColor ) {
		generate(holeR, numEdgePoints, insideColor, outsideColor);
	}
	RingGen(const float holeR, const int numEdgePoints, const color4& color) {
		generate(holeR, numEdgePoints, color, color);
	}
	RingGen(const int numEdgePoints, const color4& color) { generate(0.5, numEdgePoints, color, color); }
	//RingGen() { generate(0.5, 12, black_opaque, black_opaque); }
	~RingGen() { delete[] elements; delete[] data; delete[] indices; }
};
struct FanGen {
	//5 points, 2 colors, and 2 normals
	//2 sides, three triangles per side, three vertices per triangle, 3 attributes per vertex
	static const int numElements = 5 + 2 + 2;
	static const int numVertices = 2 * 5;
	static const int numIndices = numVertices * 3;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const int sizeData = 3 * numVertices * sizeof(vec4);
	static const int sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = numVertices * 2 * sizeof(vec4);

	void generate(const color4& insideColor, const color4& outsideColor) {
		const float init = M_PI / 4.0;
		const float theta = M_PI / 9.0;
		//set vertices
		elements[0] = ORIGIN;
		for (int i = 1; i < 5; ++i)
			elements[i] = point4(cos(init + (i - 1) * theta), sin(init + (i - 1) * theta), 0.0, 1.0);
		//set colors
		elements[5] = insideColor; elements[6] = outsideColor;
		//set normals
		elements[7] = zNorm; elements[8] = -zNorm;


		//front points
		data[0] = elements[0];
		for (int i = 1; i < 5; ++i)
			data[i] = elements[i];
		//back points
		data[5] = elements[0];
		for (int i = 1; i < 5; ++i)
			data[i + 5] = elements[5 - i];
		//colors
		data[10] = data[15] = insideColor;
		for (int i = 1; i < 5; ++i) {
			//front
			data[i + 10] = outsideColor;
			//back
			data[i + 15] = outsideColor;
		}
		//normals
		for (int i = 0; i < 5; ++i) {
			//front
			data[20 + i] = zNorm;
			//bacl
			data[25 + i] = -zNorm;
		}

		//front fan
		for (int i = 0; i < 3; ++i) {
			indices[3 * i] = 0;
			indices[3 * i + 1] = i + 1;
			indices[3 * i + 2] = i + 2;
		}
		//back fan
		for (int i = 0; i < 3; ++i) {
			indices[3 * (i + 3)] = 5;
			indices[3 * (i + 3) + 1] = i + 6;
			indices[3 * (i + 3) + 2] = i + 7;
		}
	}
	FanGen(const color4& insideColor, const color4& outsideColor) { generate(insideColor, outsideColor); }
	FanGen(const color4& color) { generate(color, color); }
	//FanGen() { generate(black_opaque, black_opaque); }
};
struct CubeGen {
	//8 points, 8 colors, 6 normals
	//6 faces, 2 triangles per face, 3 vertices per triangle, 3 attributes per vertex
	static const int numElements = 8 + 8 + 6;
	static const int numVertices = 4 * 6;
	static const int numIndices = 6 * 2 * 3;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const int sizeData = 3 * numVertices * sizeof(vec4);
	static const int sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = numVertices * 2 * sizeof(vec4);

	void generate(	const color4& color_0, const color4& color_1,
				const color4& color_2, const color4& color_3,
				const color4& color_4, const color4& color_5,
				const color4& color_6, const color4& color_7	) {
		//set vertices
		elements[0] = point4(-1.0, -1.0, 1.0, 1.0); elements[1] = point4(1.0, -1.0, 1.0, 1.0);
		elements[2] = point4(1.0, 1.0, 1.0, 1.0); elements[3] = point4(-1.0, 1.0, 1.0, 1.0);
		elements[4] = point4(-1.0, 1.0, -1.0, 1.0); elements[5] = point4(1.0, 1.0, -1.0, 1.0);
		elements[6] = point4(1.0, -1.0, -1.0, 1.0); elements[7] = point4(-1.0, -1.0, -1.0, 1.0);
		//set colors
		elements[8] = color_0; elements[9] = color_1; elements[10] = color_2; elements[11] = color_3;
		elements[12] = color_4; elements[13] = color_5; elements[14] = color_6; elements[15] = color_7;
		//normals
		elements[16] = zNorm; elements[17] = -zNorm;
		elements[18] = xNorm; elements[19] = -xNorm;
		elements[20] = yNorm; elements[21] = -yNorm;


		//points
		//front
		data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2]; data[3] = elements[3];
		//back
		data[4] = elements[4]; data[5] = elements[5]; data[6] = elements[6]; data[7] = elements[7];
		//right
		data[8] = elements[1]; data[9] = elements[6]; data[10] = elements[5]; data[11] = elements[2];
		//left
		data[12] = elements[0]; data[13] = elements[3]; data[14] = elements[4]; data[15] = elements[7];
		//top
		data[16] = elements[3]; data[17] = elements[2]; data[18] = elements[5]; data[19] = elements[4];
		//bottom
		data[20] = elements[0]; data[21] = elements[7]; data[22] = elements[6]; data[23] = elements[1];
		//colors
		//front
		data[24] = color_0; data[25] = color_1; data[26] = color_2; data[27] = color_3;
		//back
		data[28] = color_4; data[29] = color_5; data[30] = color_6; data[31] = color_7;
		//right
		data[32] = color_1; data[33] = color_6; data[34] = color_5; data[35] = color_2;
		//left
		data[36] = color_0; data[37] = color_3; data[38] = color_4; data[39] = color_7;
		//top
		data[40] = color_3; data[41] = color_2; data[42] = color_5; data[43] = color_4;
		//botom
		data[44] = color_0; data[45] = color_7; data[46] = color_6; data[47] = color_1;
		//normals
		//front
		for (int i = 48; i < 52; ++i)
			data[i] = zNorm;
		//back
		for (int i = 52; i < 56; ++i)
			data[i] = -zNorm;
		//right
		for (int i = 56; i < 60; ++i)
			data[i] = xNorm;
		//left
		for (int i = 60; i < 64; ++i)
			data[i] = -xNorm;
		//top
		for (int i = 64; i < 68; ++i)
			data[i] = yNorm;
		//bottom
		for (int i = 68; i < 72; ++i)
			data[i] = -yNorm;

		//indices
		for (int i = 0; i < 6; ++i) {
			//first triangle
			indices[i * 6] = 4 * i;
			indices[i * 6 + 1] = 4 * i + 1;
			indices[i * 6 + 2] = 4 * i + 2;
			//back triangle
			indices[i * 6 + 3] = 4 * i;
			indices[i * 6 + 4] = 4 * i + 2;
			indices[i * 6 + 5] = 4 * i + 3;
		}
	}
	void generate(	const color4& frontColor, const color4& backColor,
				const color4& rightColor, const color4& leftColor,
				const color4& topColor,   const color4& bottomColor	) {
		//set vertices
		elements[0] = point4(-1.0, -1.0, 1.0, 1.0); elements[1] = point4(1.0, -1.0, 1.0, 1.0);
		elements[2] = point4(1.0, 1.0, 1.0, 1.0); elements[3] = point4(-1.0, 1.0, 1.0, 1.0);
		elements[4] = point4(-1.0, 1.0, -1.0, 1.0); elements[5] = point4(1.0, 1.0, -1.0, 1.0);
		elements[6] = point4(1.0, -1.0, -1.0, 1.0); elements[7] = point4(-1.0, -1.0, -1.0, 1.0);
		//set colors
		elements[8] = frontColor; elements[9] = backColor;
		elements[10] = rightColor; elements[11] = leftColor;
		elements[12] = topColor; elements[13] = bottomColor;
		//normals
		elements[16] = zNorm; elements[17] = -zNorm;
		elements[18] = xNorm; elements[19] = -xNorm;
		elements[20] = yNorm; elements[21] = -yNorm;

		//points
		//front
		data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2]; data[3] = elements[3];
		//back
		data[4] = elements[4]; data[5] = elements[5]; data[6] = elements[6]; data[7] = elements[7];
		//right
		data[8] = elements[1]; data[9] = elements[6]; data[10] = elements[5]; data[11] = elements[2];
		//left
		data[12] = elements[0]; data[13] = elements[3]; data[14] = elements[4]; data[15] = elements[7];
		//top
		data[16] = elements[3]; data[17] = elements[2]; data[18] = elements[5]; data[19] = elements[4];
		//bottom
		data[20] = elements[0]; data[21] = elements[7]; data[22] = elements[6]; data[23] = elements[1];
		//colors
		//front
		for (int i = 24; i < 28; ++i)
			data[i] = frontColor;
		//back
		for (int i = 28; i < 32; ++i)
			data[i] = backColor;
		//right
		for (int i = 32; i < 36; ++i)
			data[i] = rightColor;
		//left
		for (int i = 36; i < 40; ++i)
			data[i] = leftColor;
		//top
		for (int i = 40; i < 44; ++i)
			data[i] = topColor;
		//bottom
		for (int i = 44; i < 48; ++i)
			data[i] = bottomColor;
		//normals
		//front
		for (int i = 48; i < 52; ++i)
			data[i] = zNorm;
		//back
		for (int i = 52; i < 56; ++i)
			data[i] = -zNorm;
		//right
		for (int i = 56; i < 60; ++i)
			data[i] = xNorm;
		//left
		for (int i = 60; i < 64; ++i)
			data[i] = -xNorm;
		//top
		for (int i = 64; i < 68; ++i)
			data[i] = yNorm;
		//bottom
		for (int i = 68; i < 72; ++i)
			data[i] = -yNorm;

		//indices
		for (int i = 0; i < 6; ++i) {
			//first triangle
			indices[i * 6] = 4 * i;
			indices[i * 6 + 1] = 4 * i + 1;
			indices[i * 6 + 2] = 4 * i + 2;
			//back triangle
			indices[i * 6 + 3] = 4 * i;
			indices[i * 6 + 4] = 4 * i + 2;
			indices[i * 6 + 5] = 4 * i + 3;
		}
	}
	CubeGen(	const color4& color_0, const color4& color_1,
				const color4& color_2, const color4& color_3,
				const color4& color_4, const color4& color_5,
				const color4& color_6, const color4& color_7	) {
		generate(color_0, color_1, color_2, color_3, color_4, color_5, color_6, color_7);
	}
	CubeGen(const color4& color) { generate(color, color, color, color, color, color); }
	//CubeGen() { generate(black_opaque, black_opaque, black_opaque, black_opaque, black_opaque, black_opaque); }
};
struct TetraGen {
	//4 points, 4 colors, 4 normals
	//4 sides, 3 vertices per side, 3 attributes per vertex
	static const int numElements = 4 * 3;
	static const int numVertices = numElements;
	static const int numIndices = numElements;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const int sizeData = 3 * numVertices * sizeof(vec4);
	static const int sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = numVertices * 2 * sizeof(vec4);

	void generate(	const color4& color_0, const color4& color_1,
					const color4& color_2, const color4& color_3,
					const bool uniform = false						) {
		//set vertices
		elements[0] = point4(0.0, 0.0, 1.0, 1.0);
		elements[1] = point4(0.0, 0.943, -0.3333, 1.0);
		elements[2] = point4(-0.816, -0.471, -0.3333, 1.0);
		elements[3] = point4(0.816, -0.471, -0.3333, 1.0);
		//set colors
		elements[4] = color_0; elements[5] = color_1; elements[6] = color_2; elements[7] = color_3;
		//set normals
		elements[8] = -normalize(elements[3]); elements[9] = -normalize(elements[2]);
		elements[10] = -normalize(elements[1]); elements[11] = -normalize(elements[0]);

		//points
		//left
		data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2];
		//right
		data[3] = elements[0]; data[4] = elements[3]; data[5] = elements[1];
		//bottom
		data[6] = elements[0]; data[7] = elements[2]; data[8] = elements[3];
		//back
		data[9] = elements[3]; data[10] = elements[2]; data[11] = elements[1];
		//colors
		if (!uniform) {
			//left
			data[12] = color_0; data[13] = color_1; data[14] = color_2;
			//right
			data[15] = color_0; data[16] = color_0; data[17] = color_1;
			//bottom
			data[18] = color_0; data[19] = color_2; data[20] = color_3;
			//back
			data[21] = color_3; data[22] = color_2; data[23] = color_1;
		} else {
			//left
			for (int i = 12; i < 15; ++i)
				data[i] = color_0;
			//right
			for (int i = 15; i < 18; ++i)
				data[i] = color_1;
			//bottom
			for (int i = 18; i < 21; ++i)
				data[i] = color_2;
			//back
			for (int i = 21; i < 24; ++i)
				data[i] = color_3;
		}
		//normals
		//left
		for (int i = 24; i < 27; ++i)
			data[i] = elements[8];
		//right
		for (int i = 27; i < 30; ++i)
			data[i] = elements[9];
		//bottom
		for (int i = 30; i < 33; ++i)
			data[i] = elements[10];
		//back
		for (int i = 33; i < 36; ++i)
			data[i] = elements[11];

		//indices
		for (int i = 0; i < 12; ++i)
			indices[i] = i;
	}
	TetraGen(	const color4& color_0, const color4& color_1,
				const color4& color_2, const color4& color_3,
				const bool uniform = false						) {
		generate(color_0, color_1, color_2, color_3, uniform);
	}
	TetraGen(const color4& color) { generate(color, color, color, color, true); }
};
struct CylinderGen {
	//unknown number of vertices
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(	const int numEdgePoints, const color4& frontColor,
					const color4& backColor, const color4& sideColor	) {
		int numPoints = numEdgePoints + 1;

		//2 ends, numPoints points per end, 3 colors, numEdgePoints + 2 normals
		//numEdgePoints wedges, 4 triangles per wedge, 3 vertices per triangle, 3 attributes per vertex
		numElements = 2 * numPoints + 3 + numEdgePoints + 2;
		numVertices = 2 * (numPoints + numEdgePoints);
		numIndices = numEdgePoints * 4 * 3;

		elements = new vec4[numElements];
		data = new vec4[3 * numVertices];
		indices = new GLubyte[numIndices];

		sizeData = numVertices * 3 * sizeof(vec4);
		sizeIndices = numIndices * sizeof(GLubyte);

		colors = numVertices * sizeof(vec4);
		normals = numVertices * 2 * sizeof(vec4);

		const float theta = 2.0 * M_PI / numEdgePoints;

		//set vertices
		//front circle
		elements[0] = point4(0.0, 0.0, 1.0, 1.0);
		for (int i = 0; i < numEdgePoints; ++i)
			elements[i + 1] = point4(cos(i * theta), sin(i * theta), 1.0, 1.0);
		//back circle
		elements[numPoints] = point4(0.0, 0.0, -1.0, 1.0);
		int temp = numPoints + 1;
		for (int i = 0; i < numEdgePoints; ++i)
			elements[i + temp] = point4(cos(i * theta), sin(i * theta), -1.0, 1.0);
		//set colors
		elements[2 * numPoints] = frontColor;
		elements[2 * numPoints + 1] = backColor;
		elements[2 * numPoints + 2] = sideColor;
		//set normals
		elements[2 * numPoints + 3] = zNorm;
		elements[2 * numPoints + 4] = -zNorm;
		temp = 2 * numPoints + 5;
		for (int i = 0; i < numEdgePoints; ++i)
			elements[i + temp] = normal4(cos(i * theta), sin(i * theta), 0.0, 0.0);

		//points
		//front circle
		data[0] = elements[0];
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + 1] = elements[i + 1];
		//back circle
		data[numPoints] = elements[numPoints];
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + numPoints + 1] = elements[2 * numPoints - 1 - i];
		//sides
		for (int i = 0; i < numEdgePoints; ++i) {
			data[i + 2 * numPoints] = elements[i + 1];
			data[i + 2 * numPoints + numEdgePoints] = elements[i + numPoints + 1];
		}
		//colors
		//front
		for (int i = 2 * (numPoints + numEdgePoints); i < 3 * numPoints + 2 * numEdgePoints; ++i)
			data[i] = frontColor;
		//back
		for (int i = 3 * numPoints + 2 * numEdgePoints; i < 4 * numPoints + 2 * numEdgePoints; ++i)
			data[i] = backColor;
		//side
		for (int i = 4 * numPoints + 2 * numEdgePoints; i < 4 * (numPoints + numEdgePoints); ++i)
			data[i] = sideColor;
		//normals
		//front
		for (int i = 4 * (numPoints + numEdgePoints); i < 5 * numPoints + 4 * numEdgePoints; ++i)
			data[i] = zNorm;
		//back
		for (int i = 5 * numPoints + 4 * numEdgePoints; i < 6 * numPoints + 4 * numEdgePoints; ++i)
			data[i] = -zNorm;
		//side
		temp = 6 * numPoints + 4 * numEdgePoints;
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + temp] = data[i + numEdgePoints + temp] = elements[i + 2 * numPoints + 5];

		//indices
		//front circle
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			indices[i * 3] = 0; indices[i * 3 + 1] = i + 1; indices[i * 3 + 2] = i + 2;
		}
		indices[(numEdgePoints - 1) * 3] = 0;
		indices[(numEdgePoints - 1) * 3 + 1] = numPoints - 1;
		indices[(numEdgePoints - 1) * 3 + 2] = 1;
		//back circle
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			indices[3 * (i + numEdgePoints)] = numPoints;
			indices[3 * (i + numEdgePoints) + 1] = i + 1 + numPoints;
			indices[3 * (i + numEdgePoints) + 2] = i + 2 + numPoints;
		}
		indices[3 * (2 * numEdgePoints - 1)] = numPoints;
		indices[3 * (2 * numEdgePoints - 1) + 1] = 2 * numPoints - 1;
		indices[3 * (2 * numEdgePoints - 1) + 2] = numPoints + 1;
		//side squares
		temp = 2 * numPoints;
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			//first triangle
			indices[6 * (i + numEdgePoints)] = i + 1 + temp;
			indices[6 * (i + numEdgePoints) + 1] = i + temp;
			indices[6 * (i + numEdgePoints) + 2] = i + numEdgePoints + temp;
			//second triangle
			indices[6 * (i + numEdgePoints) + 3] = i + numEdgePoints + temp;
			indices[6 * (i + numEdgePoints) + 4] = i + numEdgePoints + 1 + temp;
			indices[6 * (i + numEdgePoints) + 5] = i + 1 + temp;
		}
		//first triangle
		indices[12 * numEdgePoints - 6] = temp;
		indices[12 * numEdgePoints - 5] = numEdgePoints - 1 + temp;
		indices[12 * numEdgePoints - 4] = 2 * numEdgePoints - 1 + temp;
		//second triangle
		indices[12 * numEdgePoints - 3] = 2 * numEdgePoints - 1 + temp;
		indices[12 * numEdgePoints - 2] = numEdgePoints + temp;
		indices[12 * numEdgePoints - 1] = temp;
	}
	CylinderGen(	const int numEdgePoints, const color4& frontColor,
					const color4& backColor, const color4& sideColor	) {
		generate(numEdgePoints, frontColor, backColor, sideColor);
	}
	CylinderGen(const int numEdgePoints, const color4& endColor, const color4& sideColor) {
		generate(numEdgePoints, endColor, endColor, sideColor);
	}
	CylinderGen(const int numEdgePoints, const color4& color) {
		generate(numEdgePoints, color, color, color);
	}
	//CylinderGen() { generate(12, black_opaque, black_opaque, black_opaque); }
	~CylinderGen() { delete[] elements; delete[] data; delete[] indices; }
};
struct ConeGen {
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(const int numEdgePoints, const color4& baseColor, const color4& pointColor) {
		int numPoints = numEdgePoints + 1;
		//numEdgePoints + 2 points, 2 colors, 2 * numEdgePoints + 1 normals
		//numEdgePoints wedges, 2 triangles per wedge, 3 vertices per triangle, 3 attributes per vertex
		numElements = 3 * numEdgePoints + 5;
		numVertices = numPoints + 2 * numEdgePoints;
		numIndices = numVertices * 3;

		elements = new vec4[numElements];
		data = new vec4[3 * numVertices];
		indices = new unsigned char[numIndices];

		sizeData = numVertices * 3 * sizeof(vec4);
		sizeIndices = numIndices * sizeof(GLubyte);

		colors = numVertices * sizeof(vec4);
		normals = numVertices * 2 * sizeof(vec4);

		const float theta = 2.0 * M_PI / numEdgePoints;

		//set vertices
		for (int i = 0; i < numEdgePoints; ++i)
			elements[i] = point4(cos(i * theta), sin(i * theta), 1.0, 1.0);
		elements[numEdgePoints] = point4(0.0, 0.0, 1.0, 1.0);
		elements[numPoints] = point4(0.0, 0.0, -1.0, 1.0);
		//set colors
		elements[numEdgePoints + 2] = baseColor; elements[numEdgePoints + 3] = pointColor;
		//set normals
		elements[numEdgePoints + 4] = zNorm;
		int temp = numEdgePoints + 5;
		mat4 tempRotate;
		const normal4 tempNorm1 = normal4(0.4472, 0.0, -0.1056, 0.0);
		const normal4 tempNorm2 = rZ(theta/2.0) * tempNorm1;
		for (int i = 0; i < numEdgePoints; ++i) {
			tempRotate = rZ(i * theta);
			elements[i + temp] = tempRotate * tempNorm1;
			elements[i + numEdgePoints + temp] = tempRotate * tempNorm2;
		}

		//points
		//base
		for (int i = 0; i < numEdgePoints; ++i)
			data[i] = elements[i];
		data[numEdgePoints] = elements[numEdgePoints];
		//edge
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + numPoints] = elements[i];
		//tip
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + 2 * numEdgePoints + 1] = elements[numPoints];
		//colors
		//base
		for (int i = 3 * numEdgePoints + 1; i < 5 * numEdgePoints + 2; ++i)
			data[i] = baseColor;
		//tip
		for (int i = 5 * numEdgePoints + 2; i < 6 * numEdgePoints + 2; ++i)
			data[i] = pointColor;
		//normals
		//base
		for (int i = 6 * numEdgePoints + 2; i < 7 * numEdgePoints + 3; ++i)
			data[i] = zNorm;
		//edge
		temp = 7 * numEdgePoints + 3;
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + temp] = elements[i + numEdgePoints + 5];
		//tip
		temp = 8 * numEdgePoints + 3;
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + temp] = elements[i + 2 * numEdgePoints + 5];

		//indices
		//base
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			indices[3 * i] = numEdgePoints; indices[3 * i + 1] = i; indices[3 * i + 2] = i + 1;
		}
		indices[3 * numEdgePoints - 3] = numEdgePoints;
		indices[3 * numEdgePoints - 2] = numEdgePoints - 1;
		indices[3 * numEdgePoints - 1] = 0;
		//cone
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			indices[3 * (i + numEdgePoints)] = i + 1 + numPoints;
			indices[3 * (i + numEdgePoints) + 1] = i + numPoints;
			indices[3 * (i + numEdgePoints) + 2] = i + numPoints + numEdgePoints;
		}
		indices[6 * numEdgePoints - 3] = numPoints;
		indices[6 * numEdgePoints - 2] = numPoints + numEdgePoints - 1;
		indices[6 * numEdgePoints - 1] = 3 * numEdgePoints;
	}
	ConeGen(const int numEdgePoints, const color4& baseColor, const color4& pointColor) {
		generate(numEdgePoints, baseColor, pointColor);
	}
	ConeGen(const int numEdgePoints, const color4& color) { generate(numEdgePoints, color, color); }
	//ConeGen() { generate(12, black_opaque, black_opaque); }
	~ConeGen() { delete[] elements; delete[] data; delete[] indices; }
};
struct TubeGen {
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(	const float holeR, const int numEdgePoints,
					const color4& insideColor, const color4& outsideColor	) {
		if (holeR > 1.0 || holeR < 0.0) throw 11;

		const float theta = 2.0 * M_PI / numEdgePoints;

		//2 sides, 2 * numEdgePoints points, 2 colors, 2 * numEdgePoints + 2 normals
		//numEdgePoints wedges, 4 sides per wedge, 1 quadrilateral per side,
		//2 triangles per quadrilateral, 3 vertices per triangle, 3 attributes per vertex
		numElements = 2 * 2 * numEdgePoints + 2 + 2 * numEdgePoints + 2;
		numVertices = 2 * numEdgePoints * 4;
		numIndices = numEdgePoints * 4 * 2 * 3;

		elements = new vec4[numElements];
		data = new vec4[numVertices * 3];
		indices = new GLubyte[numIndices];

		sizeData = numVertices * 3 * sizeof(vec4);
		sizeIndices = numIndices * sizeof(GLubyte);

		colors = numVertices * sizeof(vec4);
		normals = numVertices * 2 * sizeof(vec4);

		//set vertices
		//front
		//inside
		for (int i = 0; i < numEdgePoints; ++i)
			elements[i] = point4(holeR * cos(i * theta), holeR * sin(i * theta), 1.0, 1.0);
		//outside
		for (int i = 0; i < numEdgePoints; ++i)
			elements[i + numEdgePoints] = point4(cos(i * theta), sin(i * theta), 1.0, 1.0);
		//back
		//outside
		int temp = 2 * numEdgePoints;
		for (int i = 0; i < numEdgePoints; ++i)
			elements[i + temp] = point4(cos(i * theta), sin(i * theta), -1.0, 1.0);
		//inside
		temp = 3 * numEdgePoints;
		for (int i = 0; i < numEdgePoints; ++i)
			elements[i + temp] = point4(holeR * cos(i * theta), holeR * sin(i * theta), -1.0, 1.0);
		//set colors
		elements[4 * numEdgePoints] = insideColor; elements[4 * numEdgePoints + 1] = outsideColor;
		//set normals
		elements[4 * numEdgePoints + 2] = zNorm; elements[4 * numEdgePoints + 3] = -zNorm;
		temp = 4 * (numEdgePoints + 1);
		for (int i = 0; i < numEdgePoints; ++i) {
			elements[i + temp] = normal4(cos(i * theta), sin(i * theta), 0.0, 0.0);
			elements[i + numEdgePoints + temp] = -1.0 * elements[i + temp];
		}

		//points
		//front
		for (int i = 0; i < 2 * numEdgePoints; ++i)
			data[i] = elements[i];
		//back
		for (int i = 2 * numEdgePoints; i < 4 * numEdgePoints; ++i)
			data[i] = elements[i];
		//inside
		temp = 4 * numEdgePoints;
		for (int i = 0; i < numEdgePoints; ++i) {
			data[i + temp] = elements[i + 3 * numEdgePoints];
			data[i + numEdgePoints + temp] = elements[i];
		}
		//outside
		temp = 6 * numEdgePoints;
		for (int i = 0; i < numEdgePoints; ++i) {
			data[i + temp] = elements[i + numEdgePoints];
			data[i + numEdgePoints + temp] = elements[i + 2 * numEdgePoints];
		}
		//colors
		//front
		temp = 8 * numEdgePoints;
		for (int i = 0; i < numEdgePoints; ++i) {
			data[i + temp] = insideColor;
			data[i + numEdgePoints + temp] = outsideColor;
		}
		//back
		temp = 10 * numEdgePoints;
		for (int i = 0; i < numEdgePoints; ++i) {
			data[i + temp] = outsideColor;
			data[i + numEdgePoints + temp] = insideColor;
		}
		//inside and outside
		for (int i = 12 * numEdgePoints; i < 14 * numEdgePoints; ++i) {
			//inside
			data[i] = insideColor;
			//outside
			data[i + 2 * numEdgePoints] = outsideColor;
		}
		//normals
		//front and back
		for (int i = 16 * numEdgePoints; i < 18 * numEdgePoints; ++i) {
			data[i] = zNorm;
			data[i + 2 * numEdgePoints] = -zNorm;
		}
		//inside
		temp = 20 * numEdgePoints;
		for (int i = 0; i < numEdgePoints; ++i)
			data[i + temp] = data[i + numEdgePoints + temp] = elements[i + 5 * numEdgePoints + 4];
		//outside
		temp = 22 * numEdgePoints;
		for (int i =0; i < numEdgePoints; ++i)
			data[i + temp] = data[i + numEdgePoints + temp] = elements[i + 4 * (numEdgePoints + 1)];

		//indices
		//front
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			//first triangle
			indices[i * 6] = i + 1;
			indices[i * 6 + 1] = i;
			indices[i * 6 + 2] = i + numEdgePoints;
			//second triangle
			indices[i * 6 + 3] = i + numEdgePoints;
			indices[i * 6 + 4] = i + 1 + numEdgePoints;
			indices[i * 6 + 5] = i + 1;
		}
		//first triangle
		indices[numEdgePoints * 6 - 6] = 0;
		indices[numEdgePoints * 6 - 5] = numEdgePoints - 1;
		indices[numEdgePoints * 6 - 4] = 2 * numEdgePoints - 1;
		//second triangle
		indices[numEdgePoints * 6 - 3] = 2 * numEdgePoints - 1;
		indices[numEdgePoints * 6 - 2] = numEdgePoints;
		indices[numEdgePoints * 6 - 1] = 0;
		//back
		temp = 2 * numEdgePoints;
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			//first triangle
			indices[6 * (i + numEdgePoints)] = i + 1 + temp;
			indices[6 * (i + numEdgePoints) + 1] = i + temp;
			indices[6 * (i + numEdgePoints) + 2] = i + numEdgePoints + temp;
			//second triangle
			indices[6 * (i + numEdgePoints) + 3] = i + numEdgePoints + temp;
			indices[6 * (i + numEdgePoints) + 4] = i + 1 + numEdgePoints + temp;
			indices[6 * (i + numEdgePoints) + 5] = i + 1 + temp;
		}
		//first triangle
		indices[12 * numEdgePoints - 6] = temp;
		indices[12 * numEdgePoints - 5] = numEdgePoints - 1 + temp;
		indices[12 * numEdgePoints - 4] = 2 * numEdgePoints - 1 + temp;
		//second triangle
		indices[12 * numEdgePoints - 3] = 2 * numEdgePoints - 1 + temp;
		indices[12 * numEdgePoints - 2] = numEdgePoints + temp;
		indices[12 * numEdgePoints - 1] = temp;
		//inside
		temp = 4 * numEdgePoints;
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			//first triangle
			indices[6 * (i + 2 * numEdgePoints)] = i + 1 + temp;
			indices[6 * (i + 2 * numEdgePoints) + 1] = i + temp;
			indices[6 * (i + 2 * numEdgePoints) + 2] = i + numEdgePoints + temp;
			//second triangle
			indices[6 * (i + 2 * numEdgePoints) + 3] = i + numEdgePoints + temp;
			indices[6 * (i + 2 * numEdgePoints) + 4] = i + 1 + numEdgePoints + temp;
			indices[6 * (i + 2 * numEdgePoints) + 5] = i + 1 + temp;
		}
		//first triangle
		indices[18 * numEdgePoints - 6] = temp;
		indices[18 * numEdgePoints - 5] = numEdgePoints - 1 + temp;
		indices[18 * numEdgePoints - 4] = 2 * numEdgePoints - 1 + temp;
		//second triangle
		indices[18 * numEdgePoints - 3] = 2 * numEdgePoints - 1 + temp;
		indices[18 * numEdgePoints - 2] = numEdgePoints + temp;
		indices[18 * numEdgePoints - 1] = temp;
		//outside
		temp = 6 * numEdgePoints;
		for (int i = 0; i < numEdgePoints - 1; ++i) {
			//first triangle
			indices[6 * (i + 3 * numEdgePoints)] = i + 1 + temp;
			indices[6 * (i + 3 * numEdgePoints) + 1] = i + temp;
			indices[6 * (i + 3 * numEdgePoints) + 2] = i + numEdgePoints + temp;
			//second triangle
			indices[6 * (i + 3 * numEdgePoints) + 3] = i + numEdgePoints + temp;
			indices[6 * (i + 3 * numEdgePoints) + 4] = i + 1 + numEdgePoints + temp;
			indices[6 * (i + 3 * numEdgePoints) + 5] = i + 1 + temp;
		}
		//first triangle
		indices[24 * numEdgePoints - 6] = temp;
		indices[24 * numEdgePoints - 5] = numEdgePoints - 1 + temp;
		indices[24 * numEdgePoints - 4] = 2 * numEdgePoints - 1 + temp;
		//second triangle
		indices[24 * numEdgePoints - 3] = 2 * numEdgePoints - 1 + temp;
		indices[24 * numEdgePoints - 2] = numEdgePoints + temp;
		indices[24 * numEdgePoints - 1] = temp;
	}
	TubeGen(	const float holeR, const int numEdgePoints,
					const color4& insideColor, const color4& outsideColor	) {
		generate(holeR, numEdgePoints, insideColor, outsideColor);
	}
	TubeGen(const float holeR, const int numEdgePoints, const color4& color) {
		generate(holeR, numEdgePoints, color, color);
	}
	TubeGen(const int numEdgePoints, const color4& color) {
		generate(0.5, numEdgePoints, color, color);
	}
	//TubeGen() { generate(0.5, 12, black_opaque, black_opaque); }
	~TubeGen() { delete[] elements; delete[] data; delete[] indices; }
};
struct SphereGen {
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(	const int numLatPoints, const int numLongPoints, const color4& color,
					const color4& northColor, const color4& southColor, const bool random	) {
		if (numLongPoints < 2) throw 25;

		//numLatPoints * numLongPoints + 2 points, 1 color, numLatPoints * numLongPoints + 2 normals
		//numLatPoints * (numLongPoints - 1) squares, 2 triangles per square, plus 2 cones,
		//numLatPoints triangles per cone, 3 vertices per triangle, 3 attributes per vertex
		numElements = (numLatPoints * numLongPoints + 2) * 2 + 1;
		numVertices = numLatPoints * numLongPoints + 2;
		numIndices = (numLatPoints * (numLongPoints - 1) * 2 + 2 * numLatPoints) * 3;

		elements = nullptr;
		data = new vec4[3 * numVertices];
		indices = new GLubyte[numIndices];

		sizeData = 3 * numVertices * sizeof(vec4);
		sizeIndices = numIndices * sizeof(GLubyte);

		colors = numVertices * sizeof(vec4);
		normals = 2 * numVertices * sizeof(vec4);

		const float theta = 2.0 * M_PI / numLatPoints;
		const float rho = M_PI / (numLongPoints + 1.0);

		//points
		//center
		for (int i = 0; i < numLongPoints; ++i)
			for (int j = 0; j < numLatPoints; ++j)
				data[j + i * numLatPoints] = point4(sin((i + 1) * rho) * cos(j * theta),
													sin((i + 1) * rho) * sin(j * theta),
													cos((i + 1) * rho), 1.0				);
		//top and bottom poles
		data[numLatPoints * numLongPoints] = point4(0.0, 0.0, 1.0, 1.0);
		data[numLatPoints * numLongPoints + 1] = point4(0.0, 0.0, -1.0, 1.0);
		//colors
		if (!random) {
			//center
			for (int i = numLatPoints * numLongPoints + 2; i < 2 * (numLatPoints * numLongPoints + 1); ++i)
				data[i] = color;
			//top pole
			data[2 * (numLatPoints * numLongPoints + 1)] = northColor;
			//bottom pole
			data[2 * (numLatPoints * numLongPoints + 1) + 1] = southColor;
		} else {
			for (int i = numLatPoints * numLongPoints + 2; i < 2 * (numLatPoints * numLongPoints + 2); ++i) {
				float red = (float) rand() / RAND_MAX;
				float green = (float) rand() / RAND_MAX;
				float blue = (float) rand() / RAND_MAX;
				data[i] = color4(red, green, blue, 1.0);
			}
		}
		//normals
		int temp = 2 * (numLatPoints * numLongPoints + 2);
		for (int i = 0; i < numLatPoints * numLongPoints + 2; ++i) {
			data[i + temp] = data[i]; data[i + temp].w = 0.0;
		}

		//indices
		//center
		for (int i = 0; i < numLongPoints - 1; ++i) {
			for (int j = 0; j < numLatPoints - 1; ++j) {
				//first triangle
				indices[6 * (j + i * numLatPoints)] = j + i * numLatPoints + 1;
				indices[6 * (j + i * numLatPoints) + 1] = j + i * numLatPoints;
				indices[6 * (j + i * numLatPoints) + 2] = j + (i + 1) * numLatPoints;
				//second triangle
				indices[6 * (j + i * numLatPoints) + 3] = j + (i + 1) * numLatPoints;
				indices[6 * (j + i * numLatPoints) + 4] = j + (i + 1) * numLatPoints + 1;
				indices[6 * (j + i * numLatPoints) + 5] = j + i * numLatPoints + 1;
			}
			//first triangle
			indices[6 * (1 + i) * numLatPoints - 6] = i * numLatPoints;
			indices[6 * (1 + i) * numLatPoints - 5] = (i + 1) * numLatPoints - 1;
			indices[6 * (1 + i) * numLatPoints - 4] = (i + 2) * numLatPoints - 1;
			//second triangle
			indices[6 * (1 + i) * numLatPoints - 3] = (i + 2) * numLatPoints - 1;
			indices[6 * (1 + i) * numLatPoints - 2] = (i + 1) * numLatPoints;
			indices[6 * (1 + i) * numLatPoints - 1] = i * numLatPoints;
		}
		//top cone
		temp = 6 * (numLongPoints - 1) * numLatPoints;
		for (int i = 0; i < numLatPoints - 1; ++i) {
			indices[3 * i + temp] = numLatPoints * numLongPoints;
			indices[3 * i + temp + 1] = i;
			indices[3 * i + temp + 2] = i + 1;
		}
		indices[3 * numLatPoints + temp - 3] = numLatPoints * numLongPoints;
		indices[3 * numLatPoints + temp - 2] = numLatPoints - 1;
		indices[3 * numLatPoints + temp - 1] = 0;
		//bottom cone
		temp = 3 * (numLatPoints + 2 * (numLongPoints - 1) * numLatPoints);
		for (int i = 0; i < numLatPoints - 1; ++i) {
			indices[3 * i + temp] = (numLongPoints - 1) * numLatPoints + i + 1;
			indices[3 * i + temp + 1] = (numLongPoints - 1) * numLatPoints + i;
			indices[3 * i + temp + 2] = numLatPoints * numLongPoints + 1;
		}
		indices[3 * numLatPoints + temp - 3] = (numLongPoints - 1) * numLatPoints;
		indices[3 * numLatPoints + temp - 2] = numLongPoints * numLatPoints - 1;
		indices[3 * numLatPoints + temp - 1] = numLongPoints * numLatPoints + 1;
	}
	SphereGen(	const int numLatPoints, const int numLongPoints,
					const color4& color, const color4& northColor, const color4& southColor	) {
		generate(numLatPoints, numLongPoints, color, northColor, southColor, false);
	}
	SphereGen(const int numLatPoints, const int numLongPoints, const color4& color) {
		generate(numLatPoints, numLongPoints, color, color, color, false);
	}
	//SphereGen() { generate(10, 12, black_opaque, black_opaque, black_opaque, false); }
	~SphereGen() { delete[] data; delete[] indices; }
};

//Model Animations
mat4 rotateWheel(const float theta) {
	return rZ(25 * theta);
}
mat4 driveCar(const float theta) {
	return rY(theta);
}

//Model Generators
Node* genSun(const mat4& initTrans) {
	return new Node(sunState, new Node(sunDisk), nullptr, initTrans);
}
Node* genGround(const float height = 0.0, const float size = 1.0,
				const mat4& initTrans = ID, const mat4& initSkew = ID) {
	mat4 tempTrans = translate(0.0, -height, 0.0);
	mat4 tempRotate = rX(-M_PI/2.0);
	return new Node(groundSquare, nullptr, nullptr,
		initTrans * tempTrans * scale(size) * tempRotate, initSkew * tempRotate);
}
Node* genWheel(const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* wheel = new Node(nullptr, nullptr, nullptr, initTrans, initSkew);

	mat4 rotMat = rX(M_PI/2.0);
	mat4 tempTransform = distort(0.1, 0.1, 0.6);
	wheel->addChild( new Node(wheelCylinder, nullptr, nullptr,
		rotMat * tempTransform, rotMat, rotateWheel) );
	mat4 tempRotate = rZ(M_PI/4.0);
	wheel->addChild( new Node(wheelCylinder, nullptr, nullptr,
		tempRotate * rotMat * tempTransform, tempRotate * rotMat, rotateWheel) );
	tempRotate = rZ(M_PI/2.0);
	wheel->addChild( new Node(wheelCylinder, nullptr, nullptr,
		tempRotate * rotMat * tempTransform, tempRotate * rotMat, rotateWheel) );
	tempRotate = rZ(-M_PI/4.0);
	wheel->addChild( new Node(wheelCylinder, nullptr, nullptr,
		tempRotate * rotMat *  tempTransform, tempRotate * rotMat, rotateWheel) );

	wheel->addChild( new Node(wheelTube, nullptr, nullptr, distort(1.0, 1.0, 0.2), ID, rotateWheel) );

	return wheel;
}
Node* genChassis(const mat4& initTrans = ID, const mat4& initSkew = ID, const color4& paint = red_opaque) {
	Node* chassis = new Node(nullptr, nullptr, nullptr, initTrans, initSkew);
	
	mat4 tempRotate = rX(M_PI/2.0);
	chassis->addChild( new Node(carSphere, nullptr, nullptr,
		distort(2.0, 0.5, 1.0) * tempRotate, distort(0.5, 2.0, 1.0) * tempRotate) );

	chassis->addChild( new Node(carSphere, nullptr, nullptr,
		translate(0.3, 0.25, 0.0) * distort(1.25, 0.5, 0.85), distort(0.8, 2.0, 1/0.85)) );
	
	mat4 tempSkew = distort(0.35, 0.2, 1.0);
	mat4 tempTrans = translate(-2.0, -1.0, 0.0);
	chassis->addChild( new Node(carCube, nullptr, nullptr, tempSkew * tempTrans) );
	tempTrans[0][3] = 2.85;
	chassis->addChild( new Node(carCube, nullptr, nullptr, tempSkew * tempTrans) );

	return chassis;
}
Node* genCar(const mat4& initTrans = ID, const mat4& initSkew = ID, const color4& paint = red_opaque) {
	Node* car = new Node(nullptr, genChassis(ID, ID, paint), nullptr, initTrans, initSkew);
	
	mat4 tempSkew = scale(0.3);
	mat4 tempTrans = translate(-0.7, -0.36, 0.75);
	car->addChild(genWheel(tempTrans * tempSkew));
	tempTrans[0][3] = 1.01;
	car->addChild(genWheel(tempTrans * tempSkew));
	tempTrans[2][3] = -0.75;
	car->addChild(genWheel(tempTrans * tempSkew));
	tempTrans[0][3] = -0.7;
	car->addChild(genWheel(tempTrans * tempSkew));

	return car;
}
Node* genRoad(const mat4& initTrans = ID, const mat4& initSkew = ID, const float thinness = 1.0) {
	mat4 tempRotate = rX(-M_PI/2.0);
	return new Node(roadRing, nullptr, nullptr, initTrans * tempRotate, initSkew * tempRotate);
}
Node* genDrivingScene(const float thinness = 1.0, const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* road = new Node(nullptr, genRoad(ID, ID, thinness), nullptr, initTrans, initSkew);
	const float width = 1.0 - 0.5 * thinness;
	mat4 tempTrans = translate(0.0, 0.1 * width, -0.5 * (thinness + width) + 0.1 * width);
	road->addChild( new Node(nullptr, genCar(tempTrans * scale(0.15 * width)), nullptr, ID, ID, driveCar) );
	return road;
}
Node* genLeaf(const float prob = 0.5, const mat4& initTrans = ID, const mat4& initSkew = ID) {
	const int colorInd = bernoulli() < prob ? 1 : 0;
	return new Node(leaf[colorInd], nullptr, nullptr, initTrans * rZ(M_PI/12.0), initSkew);
}
Node* genLeafBunch(const float rho = M_PI/3.0, const float radius = 1.0, const float prob = 0.5,
				const int numLeaves = 10, const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* bunch = new Node(nullptr, nullptr, nullptr, initTrans, initSkew);

	mat4 rotMat = ID;
	mat4 tempTrans = translate(0.0, radius, 0.0);
	mat4 tempRotate = rY( randNum(0.0, 2.0 * M_PI) ) * rZ( randNum(0.0, rho) );
	bunch->addChild( new Node(nullptr, genLeaf(prob), nullptr, tempRotate * tempTrans, tempRotate) );
	for (int i = 1; i < numLeaves; ++i) {
		rotMat = rY( randNum(0.0, 2.0 * M_PI) ) * rX( randNum(0.0, M_PI/2.0) );
		tempRotate = rY( randNum(0.0, 2.0 * M_PI) ) * rX( randNum(0.0, rho) );
		bunch->addChild( new Node(nullptr, genLeaf(prob), nullptr, tempRotate * tempTrans * rotMat, tempRotate) );
	}

	return bunch;
}
Node* genBranch(const int branching = 3, const int maxDepth = 4, const int density = 10, const float scaleFactor = 0.6,
				const float seasonProb = 0.5, const mat4& initTrans = ID, const mat4& initSkew = ID,
				const int currentDepth = 0) {
	mat4 tempTrans = distort(0.2, 1.0, 0.2);
	mat4 tempMove = translate(0.0, 1.0, 0.0);
	mat4 tempSkew = distort(5.0, 1.0, 5.0);
	mat4 tempRotate = rX(M_PI/2.0);
	Node* branch = new Node(nullptr, nullptr, nullptr, initTrans, initSkew);
	branch->addChild(new Node( treeCone, nullptr, nullptr,
		tempMove * tempTrans * tempRotate, tempSkew * tempRotate ));

	if (currentDepth < maxDepth) {
		const int branchFact = branching;
		mat4 tempRotate = rZ(M_PI/6.0);
		mat4 rotMat = rY(2.0 * M_PI / branchFact);
		tempTrans = scale(scaleFactor);
		tempMove = translate(0.0, 1.5, 0.0);
		mat4 twist = rY(M_PI / branchFact) * tempMove;
		mat4 loopTemp = rotMat * tempRotate;

		for (int i = 0; i < branchFact; ++i) {
			loopTemp = rotMat * loopTemp;
			branch->addChild( genBranch( branching, maxDepth, density, scaleFactor, seasonProb,
				twist * loopTemp * tempTrans, loopTemp, currentDepth + 1 ) );
		}
	}
	
	return branch;
}
Node* genTree(const int branching = 3, const int maxDepth = 2, const int density = 15, const float scaleFactor = 0.6,
				const float seasonProb = 0.15, const float height = 1.6,
				const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* tree = new Node(nullptr, nullptr, nullptr, initTrans, initSkew);

	mat4 tempRotate = rX(M_PI/2.0);
	mat4 tempTrans = distort(1.0, 0.2, 1.0);
	mat4 tempSkew = distort(1.0, 5.0, 1.0);
	mat4 tempMove = translate(0.0, -2.0, 0.0);
	tree->addChild( new Node(treeCone, nullptr, nullptr,
		tempMove * tempTrans * tempRotate, tempSkew * tempRotate) );

	tempTrans = distort(0.3, height, 0.3);
	tempMove[1][3] = height - 2.0;
	tree->addChild( new Node(treeCylinder, nullptr, nullptr,
		tempMove * tempTrans * tempRotate, tempRotate) );

	tempMove[1][3] = height * 0.6 + 0.1;
	tree->addChild( genBranch(branching, maxDepth, density, scaleFactor, seasonProb, tempMove) );

	mat4 rotMat = rY(2.0 * M_PI/4.0);
	tempSkew = rZ(M_PI/4.0);
	for (int i = 0; i < 4; ++i) {
		tempSkew = rotMat * tempSkew;
		tree->addChild( genBranch(branching, maxDepth, density, scaleFactor, seasonProb,
			tempMove * tempSkew, tempSkew) );
	}

	tree->addChild( genLeafBunch(M_PI/2.0, 3.0, 0.5, density, translate(0.0, height, 0.0) * scale(0.7)) );

	return tree;
}
Node* genForest(const int numTrees = 10, const float minR = 5.0, const float maxR = 15.0,
				const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* forest = new Node(nullptr, nullptr, nullptr, initTrans, initSkew);

	mat4 rotMat = rY(0.35 * numTrees + 2.0 * M_PI / numTrees);
	mat4 tempPosit = ID;
	for (int i = 0; i < numTrees; ++i, rotMat *= rotMat) {
		tempPosit = rotMat * translate(0.0, 0.0, -randNum(minR, maxR));
		forest->addChild( genTree( 2, 1, 100, 0.5, 0.5, 1.6, tempPosit, tempPosit ) );
	}

	return forest;
}
Node* genFirePlace(const mat4& initTrans = ID, const mat4& initSkew = ID) {
	return new Node(furnTube, nullptr, nullptr, initTrans * rZ(M_PI/4.0), initSkew * rZ(M_PI/4.0));
}
Node* genBed(const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* bed = new Node(nullptr, nullptr, nullptr, initTrans, initSkew);

	bed->addChild( new Node(furnCube0, nullptr, nullptr, translate(0.5, 0.0, 0.0) * distort(0.3, 0.2, 0.4)) );
	bed->addChild( new Node(furnCube1, nullptr, nullptr, translate(-0.5, 0.0, 0.0) * distort(0.7, 0.2, 0.4)) );
	bed->addChild( new Node(treeCylinder, nullptr, nullptr,
		translate(-1.1, -0.3, 0.3) * scale(0.1) * rX(M_PI/2.0), rX(M_PI/2.0)) );
	bed->addChild( new Node(treeCylinder, nullptr, nullptr,
		translate(0.7, -0.3, 0.3) * scale(0.1) * rX(M_PI/2.0), rX(M_PI/2.0)) );
	bed->addChild( new Node(treeCylinder, nullptr, nullptr,
		translate(0.7, -0.3, -0.3) * scale(0.1) * rX(M_PI/2.0), rX(M_PI/2.0)) );
	bed->addChild( new Node(treeCylinder, nullptr, nullptr,
		translate(-1.1, -0.3, -0.3) * scale(0.1) * rX(M_PI/2.0), rX(M_PI/2.0)) );

	return bed;
}
Node* genChair(const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* chair = new Node(nullptr, nullptr, nullptr, initTrans, initSkew);

	chair->addChild( new Node(furnCube1, nullptr, nullptr, distort(0.5, 0.2, 0.4)) );
	chair->addChild( new Node(furnCube1, nullptr, nullptr,
		translate(-0.5, 0.5, 0.0) * rZ(M_PI/8.0) * distort(0.15, 0.45, 0.4), rZ(M_PI/8.0)) );
	chair->addChild( new Node(furnCube1, nullptr, nullptr,
		translate(0.0, 0.3, 0.45) * distort(0.4, 0.1, 0.1)) );
	chair->addChild( new Node(furnCube1, nullptr, nullptr,
		translate(0.0, 0.3, -0.45) * distort(0.4, 0.1, 0.1)) );
	chair->addChild( new Node(treeCylinder, nullptr, nullptr,
		translate(-0.4, -0.3, 0.3) * scale(0.1) * rX(M_PI/2.0), rX(M_PI/2.0)) );
	chair->addChild( new Node(treeCylinder, nullptr, nullptr,
		translate(0.4, -0.3, 0.3) * scale(0.1) * rX(M_PI/2.0), rX(M_PI/2.0)) );
	chair->addChild( new Node(treeCylinder, nullptr, nullptr,
		translate(0.4, -0.3, -0.3) * scale(0.1) * rX(M_PI/2.0), rX(M_PI/2.0)) );
	chair->addChild( new Node(treeCylinder, nullptr, nullptr,
		translate(-0.4, -0.3, -0.3) * scale(0.1) * rX(M_PI/2.0), rX(M_PI/2.0)) );

	return chair;
}
Node* genHouseOut(const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* house = new Node(nullptr, nullptr, nullptr, initTrans, initSkew);

	const float height = 0.5;
	house->addChild( new Node(houseCube, nullptr, nullptr, translate(0.0, -height, 0.0) * distort(1.0, 0.01, 1.0)) );
	house->addChild( new Node(houseCube, nullptr, nullptr, translate(0.0, 0.0, -1.0) * distort(1.0, height, 0.01)) );
	house->addChild( new Node(houseCube, nullptr, nullptr, translate(1.0, 0.0, 0.0) * distort(0.01, height, 1.0)) );
	house->addChild( new Node(houseCube, nullptr, nullptr, translate(-1.0, 0.0, 0.0) * distort(0.01, height, 1.0)) );
	house->addChild( new Node(houseCube, nullptr, nullptr, translate(0.58, 0.0, 1.0) * distort(0.42, height, 0.01)) );
	house->addChild( new Node(houseCube, nullptr, nullptr,
		translate(-0.58, 0.0, 1.0) * distort(0.42, height, 0.01)) );
	house->addChild( new Node(houseCube, nullptr, nullptr,
		translate(0.0, 0.35, 1.0) * distort(0.25, 0.3 * height, 0.01)) );
	house->addChild( new Node(houseCube, nullptr, nullptr,
		translate(-0.29, -0.16, 1.13) * rY(M_PI/4.0) * distort(0.2, 0.7 * height, 0.01), rY(M_PI/4.0)) );
	house->addChild( new Node(housePyramid, nullptr, nullptr,
		translate(0.0, 0.85, 0.0) * scale(1.7) * distort(1.0, 0.2, 1.0) * rY(M_PI/4.0) * rX(M_PI/2.0),
		distort(1.0, 5.0, 1.0) * rY(M_PI/4.0) * rX(M_PI/2.0)) );

	return house;
}
Node* genHouseIn(const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* house = new Node(insideState, nullptr, nullptr, initTrans, initSkew);
	house->addChild( new Node(light, nullptr, nullptr, translate(0.0, -0.35, -0.87)) );

	const float height = 0.5;
	house->addChild( new Node(houseSquare, nullptr, nullptr,
		translate(0.0, -height + 0.02, 0.0) * rX(M_PI/2.0), rX(M_PI/2.0)) );
	house->addChild( new Node(houseSquare, nullptr, nullptr,
		translate(0.0, height - 0.02, 0.0) * rX(M_PI/2.0), rX(M_PI/2.0)) );
	house->addChild( new Node(houseSquare, nullptr, nullptr,
		translate(0.0, 0.0, -0.98) * distort(1.0, height, 1.0)) );
	house->addChild( new Node(houseSquare, nullptr, nullptr,
		translate(0.98, 0.0, 0.0) * distort(1.0, height, 1.0) * rY(M_PI/2.0), rY(M_PI/2.0)) );
	house->addChild( new Node(houseSquare, nullptr, nullptr,
		translate(-0.98, 0.0, 0.0) * distort(1.0, height, 1.0) * rY(M_PI/2.0), rY(M_PI/2.0)) );
	house->addChild( new Node(houseSquare, nullptr, nullptr,
		translate(0.58, 0.0, 0.98) * distort(0.42, height, 1.0)) );
	house->addChild( new Node(houseSquare, nullptr, nullptr,
		translate(-0.58, 0.0, 0.98) * distort(0.42, height, 1.0)) );
	house->addChild( new Node(houseSquare, nullptr, nullptr,
		translate(0.0, 0.35, 0.98) * distort(0.25, 0.3 * height, 1.0)) );

	house->addChild( new Node(houseCube, nullptr, nullptr,
		translate(-0.2, 0.0, 0.6) * distort(0.01, height, 0.4)) );
	house->addChild( new Node(houseCube, nullptr, nullptr,
		translate(0.2, 0.0, 0.6) * distort(0.01, height, 0.4)) );
	house->addChild( new Node(houseCube, nullptr, nullptr,
		translate(-0.75, 0.0, 0.2) * distort(0.25, height, 0.01)) );
	house->addChild( new Node(houseCube, nullptr, nullptr,
		translate(0.75, 0.0, 0.2) * distort(0.25, height, 0.01)) );

	house->addChild( genFirePlace( translate(0.0, -0.35, -0.95) *
		distort(1.5, 1.25, 0.2) * scale(0.25) ));
	house->addChild( genBed(translate(-0.5, -0.37, 0.85) * scale(0.3)) );
	house->addChild( genBed(translate(0.65, -0.37, 0.85) * scale(0.3)) );
	house->addChild( genChair(translate(-0.3, -0.4, -0.35) *
		rY(M_PI/4.0) * scale(0.2), rY(M_PI/4.0)) );
	house->addChild( genChair(translate(0.25, -0.4, -0.3) *
		rY(M_PI/2.0) * scale(0.2), rY(M_PI/2.0)) );
	house->addChild( genChair(translate(0.8, -0.4, -0.4) *
		rY(M_PI) * distort(1.0, 1.0, 3.0) * scale(0.2), rY(M_PI)) );

	return house;
}
Node* genHouse(const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* house = new Node(nullptr, genHouseOut(), nullptr, initTrans, initSkew);
	house->addChild(genHouseIn());
	return house;
}
Node* genIdyl(const int numRows = 2, const float thinness = 1.0,
				const mat4& initTrans = ID, const mat4& initSkew = ID) {
	Node* scene = new Node(outsideState, nullptr, nullptr, initTrans, initSkew);
	scene->addChild( genDrivingScene(thinness) );
	scene->addChild( genGround(0.001, 5.0) );
	mat4 tempRotate = ID;
	for (int i = 0, numTrees = 5; i < numRows; ++i, numTrees += 5) {
		scene->addChild( genForest(numTrees, 11.0 + i * 5.0, 11.0 + (i + 1) * 5.0,
		translate(0.0, 0.32, 0.0) * scale(0.15) ));
	}
	scene->addChild( genHouse(translate(0.0, 0.1, 0.0) * scale(0.2)) );

	return scene;
}

void em_MeshSetUp(const color4& paint = red_opaque, const float thinness = 1.0) {
	GLuint vao[17];
	GLuint vbo[17];
	GLuint vea[17];
	GLuint posit_loc = glGetAttribLocation(currentProg, "vPosition");
	GLuint color_loc = glGetAttribLocation(currentProg, "vColor");
	GLuint norm_loc = glGetAttribLocation(currentProg, "vNormal");
	glGenVertexArrays(17, vao);
	glGenBuffers(17, vbo);
	glGenBuffers(17, vea);
	
	//Ground
	const SquareGen square = SquareGen(darkgreen_opaque);
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, square.sizeData, square.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(square.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(square.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(square.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, square.sizeIndices, square.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	groundSquare = new Primitive(vao[0], square.numIndices);

	//Wheel
	const CylinderGen cylinder = CylinderGen(5, steel_opaque);
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, cylinder.sizeData, cylinder.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cylinder.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cylinder.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(cylinder.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinder.sizeIndices, cylinder.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	wheelCylinder = new Primitive(vao[1], cylinder.numIndices);

	const TubeGen tube = TubeGen(0.5, 30, steel_opaque, black_opaque);
	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, tube.sizeData, tube.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(tube.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(tube.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(tube.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tube.sizeIndices, tube.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	wheelTube = new Primitive(vao[2], tube.numIndices);

	//Chassis
	const SphereGen sphere = SphereGen(12, 4, paint);
	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sphere.sizeData, sphere.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sphere.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sphere.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(sphere.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.sizeIndices, sphere.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	carSphere = new Primitive(vao[3], sphere.numIndices);

	const CubeGen cube = CubeGen(paint);
	glBindVertexArray(vao[4]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, cube.sizeData, cube.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(cube.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.sizeIndices, cube.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	carCube = new Primitive(vao[4], cube.numIndices);

	//Road
	const RingGen ring = RingGen(thinness * 0.5, 30, black_opaque);
	glBindVertexArray(vao[5]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, ring.sizeData, ring.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(ring.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(ring.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(ring.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[5]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ring.sizeIndices, ring.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	roadRing = new Primitive(vao[5], ring.numIndices);

	//Leaf
	const FanGen fan1 = FanGen(darkgreen_opaque, yellow_opaque);
	glBindVertexArray(vao[6]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, fan1.sizeData, fan1.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fan1.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fan1.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(fan1.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[6]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fan1.sizeIndices, fan1.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	leaf[0] = new Primitive(vao[6], fan1.numIndices);

	const FanGen fan2 = FanGen(yellow_opaque, red_opaque);
	glBindVertexArray(vao[7]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, fan2.sizeData, fan2.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fan2.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fan2.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(fan2.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[7]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fan2.sizeIndices, fan2.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	leaf[1] = new Primitive(vao[7], fan2.numIndices);

	//Tree
	const ConeGen cone = ConeGen(4, darkbrown_opaque);
	glBindVertexArray(vao[8]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ARRAY_BUFFER, cone.sizeData, cone.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cone.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cone.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(cone.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[8]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cone.sizeIndices, cone.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	treeCone = new Primitive(vao[8], cone.numIndices);

	const CylinderGen cylinder2 = CylinderGen(8, darkbrown_opaque);
	glBindVertexArray(vao[9]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glBufferData(GL_ARRAY_BUFFER, cylinder2.sizeData, cylinder2.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cylinder2.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cylinder2.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(cylinder2.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[9]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinder2.sizeIndices, cylinder2.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	treeCylinder = new Primitive(vao[9], cylinder2.numIndices);

	CircleGen circle = CircleGen(16, white_opaque);
	glBindVertexArray(vao[10]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	glBufferData(GL_ARRAY_BUFFER, circle.sizeData, circle.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(circle.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(circle.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(circle.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[10]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, circle.sizeIndices, circle.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	sunDisk = new Primitive(vao[10], circle.numIndices);

	const CubeGen cube2 = CubeGen(darkbrown_opaque);
	glBindVertexArray(vao[11]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
	glBufferData(GL_ARRAY_BUFFER, cube2.sizeData, cube2.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube2.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube2.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(cube2.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[11]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube2.sizeIndices, cube2.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	houseCube = new Primitive(vao[11], cube2.numIndices);

	const ConeGen tetra = ConeGen(4, darkred_opaque);
	glBindVertexArray(vao[12]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
	glBufferData(GL_ARRAY_BUFFER, tetra.sizeData, tetra.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(tetra.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(tetra.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(tetra.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[12]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tetra.sizeIndices, tetra.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	housePyramid = new Primitive(vao[12], tetra.numIndices);

	const SquareGen square2 = SquareGen(darkbrown_opaque);
	glBindVertexArray(vao[13]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
	glBufferData(GL_ARRAY_BUFFER, square2.sizeData, square2.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(square2.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(square2.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(square2.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[13]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, square2.sizeIndices, square2.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	houseSquare = new Primitive(vao[13], square2.numIndices);

	const TubeGen tube2 = TubeGen(4, darkred_opaque);
	glBindVertexArray(vao[14]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);
	glBufferData(GL_ARRAY_BUFFER, tube2.sizeData, tube2.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(tube2.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(tube2.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(tube2.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[14]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tube2.sizeIndices, tube2.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	furnTube = new Primitive(vao[14], tube2.numIndices);

	const CubeGen cube3 = CubeGen(white_opaque);
	glBindVertexArray(vao[15]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[15]);
	glBufferData(GL_ARRAY_BUFFER, cube3.sizeData, cube3.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube3.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube3.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(cube3.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[15]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube3.sizeIndices, cube3.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	furnCube0 = new Primitive(vao[15], cube3.numIndices);

	const CubeGen cube4 = CubeGen(blue_opaque);
	glBindVertexArray(vao[16]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[16]);
	glBufferData(GL_ARRAY_BUFFER, cube4.sizeData, cube4.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(posit_loc);
	glVertexAttribPointer(posit_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube4.points));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube4.colors));
	glEnableVertexAttribArray(norm_loc);
	glVertexAttribPointer(norm_loc, 4, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(cube4.normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vea[16]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube4.sizeIndices, cube4.indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	furnCube1 = new Primitive(vao[16], cube4.numIndices);
}

//reshapes the viewport so that it is square and in the middle of the window
void em_Reshape(int w, int h) {
	ww = w;
	wh = h;
	if (w < h) {
		viewDim = w;
		viewx = 0;
		viewy = h / 2 - viewDim / 2; 
	}
	else if (h < w) {
		viewDim = h;
		viewy = 0;
		viewx = w / 2 - viewDim / 2;
	}
	else {
		viewDim = w;
		viewy = 0;
		viewx = 0;
	}
	glViewport(viewx, viewy, viewDim, viewDim);
}

//animates the scene
void em_Timer(int t) {
	if (animating) {
		const mat4 tempRotate = rZ(M_PI/2000.0);
		sunVec = tempRotate * sunVec;
		sun->transform(tempRotate);
		root->animate(M_PI/400.0);
		glutPostRedisplay();
	}
	glutTimerFunc(20, em_Timer, 0);
}

void em_Keyboard(unsigned char key, int x, int y) {
	//exit on Esc key
    if (key == 033) {
		//if (root != nullptr) delete root;
	    exit( EXIT_SUCCESS );

	}

	//restor to beginning on R
	else if (key == 'r' || key == 'R') {
		positMat = ID;
		pitchMat = ID;
		rollMat = ID;
		yawMat = ID;
		animating = true;
		glutPostRedisplay();
	}

	//roll right on E
	else if (key == 'e' || key == 'E') {
		rollMat = rZ(M_PI/12.0) * rollMat;
		glutPostRedisplay();
	}
	//roll left on Q
	else if (key == 'q' || key == 'Q') {
		rollMat = rZ(-M_PI/12.0) * rollMat;
		glutPostRedisplay();
	}

	//move forward on W
	else if (key == 'w' || key == 'W') { positMat[2][3] += speed; glutPostRedisplay(); }
	//move backward on S
	else if (key == 's' || key == 'S') { positMat[2][3] -= speed; glutPostRedisplay(); }
	//move left on A
	else if (key == 'a' || key == 'A') { positMat[0][3] += speed; glutPostRedisplay(); }
	//move right on D
	else if (key == 'd' || key == 'D') { positMat[0][3] -= speed; glutPostRedisplay(); }

	//crouch on C
	else if (key == 'c' || key == 'C') {
		if (crouching) { positMat[1][3] = 0.0; crouching = false; }
		else { positMat[1][3] = 2.0; crouching = true; }
		glutPostRedisplay();
	}

	//scale positMat on I (zoom in)
	else if (key == 'i' || key == 'I') {
		positMat = scale(zoomin) * positMat;
		glutPostRedisplay();
	}
	//scale down positMat on O (zoom out)
	else if (key == 'o' || key == 'O') {
		positMat = scale(zoomout) * positMat;
		glutPostRedisplay();
	}

	//Pause and play animation on T
	else if (key == 't' || key == 'T') animating = !animating;
}

void em_Mouse(int button, int state, int p, int q) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		leftMouse = true;
		pRef = p, qRef = q;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		leftMouse = false;
}

//rotates the camera if the left mouse button is pressed down.
//assumes user is clicking and pulling on a plane parallel to the projection plane,
//and approximates the angle of rotation by using the sphere tangent to that plane.
//for the plane at z = -1, that is the unit sphere, along which all arcs are equal
//to the angle that subtends them, meaning that the distance moved by the mouse
//along the tangent plane can be directly plugged in as the angle of rotation
void em_MouseMotion(int p, int q) {
	if (leftMouse) {
		float delta_p = 2.5 * FOV * (M_PI/180.0) * (p - pRef) / viewDim;
		float delta_q = 2.0 * FOV * (M_PI/180.0) * (q - qRef) / viewDim;
		mat4 delta_yaw = rY(delta_p);
		pitchTheta = (pitchTheta + delta_q > -M_PI/2.0 && pitchTheta + delta_q < M_PI/2.0) ?
			pitchTheta + delta_q : pitchTheta;
		yawMat = delta_yaw * yawMat;
		positMat = delta_yaw * positMat;
		pitchMat = rX(pitchTheta);
		pRef = p, qRef = q;
		glutPostRedisplay();
	}
}

void em_Help() {
	std::cout <<	"Clicking in the window with the left button\n" <<
					"and holding it down will allow you to turn around\n" <<
					"and look up and down based on where you drag the\n" <<
					"mouse. (Looking up/down is clamped between straight\n" <<
					"up and straight down.)\n" <<
					std::endl <<
					"Press W to walk forward, S to walk backwards,\n" <<
					"A to walk sideways left, and D to walk sideways right.\n" <<
					"Press Q and E to roll the camera left or right.\n" <<
					"Press C to crouch or stand up. Press I and O to zoom in\n" <<
					"and out, respectively. Press T to start or stops the\n" <<
					"animation. Press R to reset.\n" <<
					"Press Esc to exit.\n";
}

void em_Init() {
	srand(time(NULL));
	for (int i = 0; i < 50; ++i) rand();

	// Load shaders and use the resulting shader program
    currentProg = InitShader("em_VertexShader.glsl", "em_FragShader.glsl");
    glUseProgram(currentProg);

	camera_loc = glGetUniformLocation(currentProg, "Camera");
	trans_loc = glGetUniformLocation(currentProg, "vTrans");
	skew_loc = glGetUniformLocation(currentProg, "nTrans");
	sun_loc = glGetUniformLocation(currentProg, "sunDirect");
	isSun_loc = glGetUniformLocation(currentProg, "isSun");
	isInside_loc = glGetUniformLocation(currentProg, "isInside");

	em_MeshSetUp(red_opaque, 1.2);
	light = new PointLight("lightPosit");
	sunState = new BoolState(0.0, 1.0);
	insideState = new BoolState(1.0, 0.0);
	outsideState = new BoolState(0.0, 0.0);

	root = genIdyl(3, 1.2, translate(20.0, -6.5, -50.0) * scale(100.0));
	sun = genSun( translate(2000.0 * sunVec.x, 2000.0 * sunVec.y, 2000.0 * sunVec.z) *
		scale(65.0) * rX(M_PI/2.0) );

	glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

	em_Help();
}

void em_Display() {
	const float tempDirect = pow(abs(sunVec.y), 0.23);
	const float tempBright = (sunVec.y > 0.0) ? 1.0 : 1.0 - pow(abs(sunVec.y), 0.45);
	glClearColor(	tempBright * 0.6,
					tempBright * (0.25 + 0.35 * tempDirect),
					tempBright * (0.2 + 0.8 * tempDirect),
					1.0	);
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUniformMatrix4fv(camera_loc, 1, GL_TRUE, projMat * pitchMat * rollMat * yawMat);
	if (sun != nullptr) sun->draw();

	glUniformMatrix4fv(camera_loc, 1, GL_TRUE, projMat * pitchMat * rollMat * positMat);
	glUniform4fv(sun_loc, 1, sunVec);
	if (root != nullptr) root->draw();
	
	glutSwapBuffers();
	glFlush();
}

void m_glewInitAndVersion(void) {
	fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	GLenum err = glewInit();
	if (GLEW_OK != err) fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

int main( int argc, char **argv ) {
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( viewDim, viewDim );
    glutCreateWindow("Eric Massengill Homework 4");
	m_glewInitAndVersion();

    em_Init();

    glutDisplayFunc(em_Display);
    glutKeyboardFunc(em_Keyboard);
	//pass mouse function to glut
	glutMouseFunc(em_Mouse);
	glutMotionFunc(em_MouseMotion);
	//pass reshape function to glut
	glutReshapeFunc(em_Reshape);
	//pass animation function to glut
	glutTimerFunc(500, em_Timer, 0);

    glutMainLoop();

    return 0;
}