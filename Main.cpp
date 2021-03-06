//Begin Headers
#include "Global.h"
#include "MatMath.h"
#include "MeshGen.h"
#include "ModelGen.h"

#include "Object.h"
#include "Primitive.h"
#include "Lightsource.h"
#include "BoolState.h"

#include "RenderGraph.h"
#include "Node.h"
#include "ObjectNode.h"

#include <stdlib.h>
#include <time.h>
#include <math.h>
//End Headers

void meshSetup(const color4& paint = Global::red_opaque, const float thinness = 1.0) {
	GLuint vao[17];
	GLuint vbo[17];
	GLuint vea[17];
	GLuint occ[17];

	GLuint posit_loc = GLuint(0);
	GLuint color_loc = GLuint(1);
	GLuint norm_loc = GLuint(2);

	glGenVertexArrays(17, vao);
	glGenBuffers(17, vbo);
	glGenBuffers(17, vea);
	glGenQueries(17, occ);

	
	//Ground
	const SquareGen square = SquareGen(Global::darkgreen_opaque);
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
	Global::groundSquare = new Primitive(vao[0], square.numIndices, occ[0]);

	//Wheel
	const CylinderGen cylinder = CylinderGen(5, Global::steel_opaque);
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
	Global::wheelCylinder = new Primitive(vao[1], cylinder.numIndices, occ[1], 12.0);

	const TubeGen tube = TubeGen(0.5, 30, Global::steel_opaque, Global::black_opaque);
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
	Global::wheelTube = new Primitive(vao[2], tube.numIndices, occ[2]);

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
	Global::carSphere = new Primitive(vao[3], sphere.numIndices, occ[3], 20.0);

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
	Global::carCube = new Primitive(vao[4], cube.numIndices, occ[4], 20.0);

	//Road
	const RingGen ring = RingGen(thinness * 0.5, 30, Global::black_opaque);
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
	Global::roadRing = new Primitive(vao[5], ring.numIndices, occ[5]);

	//Leaf
	const FanGen fan1 = FanGen(Global::darkgreen_opaque, Global::yellow_opaque);
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
	Global::leaf[0] = new Primitive(vao[6], fan1.numIndices, occ[6]);

	const FanGen fan2 = FanGen(Global::yellow_opaque, Global::red_opaque);
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
	Global::leaf[1] = new Primitive(vao[7], fan2.numIndices, occ[7]);

	//Tree
	const ConeGen cone = ConeGen(4, Global::darkbrown_opaque);
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
	Global::treeCone = new Primitive(vao[8], cone.numIndices, occ[8]);

	const CylinderGen cylinder2 = CylinderGen(8, Global::darkbrown_opaque);
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
	Global::treeCylinder = new Primitive(vao[9], cylinder2.numIndices, occ[9]);

	CircleGen circle = CircleGen(16, Global::white_opaque);
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
	Global::sunDisk = new Primitive(vao[10], circle.numIndices, occ[10]);

	const CubeGen cube2 = CubeGen(Global::darkbrown_opaque);
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
	Global::houseCube = new Primitive(vao[11], cube2.numIndices, occ[11]);

	const ConeGen tetra = ConeGen(4, Global::darkred_opaque);
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
	Global::housePyramid = new Primitive(vao[12], tetra.numIndices, occ[12]);

	const SquareGen square2 = SquareGen(Global::darkbrown_opaque);
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
	Global::houseSquare = new Primitive(vao[13], square2.numIndices, occ[13]);

	const TubeGen tube2 = TubeGen(4, Global::darkred_opaque);
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
	Global::furnTube = new Primitive(vao[14], tube2.numIndices, occ[14]);

	const CubeGen cube3 = CubeGen(Global::white_opaque);
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
	Global::furnCube0 = new Primitive(vao[15], cube3.numIndices, occ[15]);

	const CubeGen cube4 = CubeGen(Global::blue_opaque);
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
	Global::furnCube1 = new Primitive(vao[16], cube4.numIndices, occ[16]);
}

void reshape(int w, int h)
//reshapes the viewport so that it is square and in the middle of the window
{
	Global::ww = w;
	Global::wh = h;
	if (w < h) {
		Global::viewDim = w;
		Global::viewx = 0;
		Global::viewy = h / 2 - Global::viewDim / 2; 
	}
	else if (h < w) {
		Global::viewDim = h;
		Global::viewy = 0;
		Global::viewx = w / 2 - Global::viewDim / 2;
	}
	else {
		Global::viewDim = w;
		Global::viewy = 0;
		Global::viewx = 0;
	}
	glViewport(Global::viewx, Global::viewy, Global::viewDim, Global::viewDim);
}

void timer(int t)
//animates the scene
{
	if (Global::animating) {
		Global::sunVec = rZ(M_PI/2000.0) * Global::sunVec;
		if (Global::sun != nullptr) Global::sun->animate(M_PI/250.0);
		//By keeping a pointer to the car scene, I can call animate() on it without incurring the overhead
		//of calling the function recursively on the rest of the object tree, Global::when only the car is animated.
		if (Global::root != nullptr) Global::root->animate(M_PI/250.0);
		glutPostRedisplay();
	}
	glutTimerFunc(20, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {

	//exit on Esc key
	case 033:
		exit( EXIT_SUCCESS );
		return;

	//restor to beginning on R
	case 'r': case 'R':
		Global::positMat = Global::initPosit;
		Global::pitchMat = Global::rollMat = Global::yawMat = MatMath::ID;
		Global::animating = true;
		glutPostRedisplay();
		return;

	//roll right on E
	case 'e': case 'E':
		Global::rollMat = rZ(M_PI/12.0) * Global::rollMat;
		glutPostRedisplay();
		return;

	//roll left on Q
	case 'q': case 'Q':
		Global::rollMat = rZ(-M_PI/12.0) * Global::rollMat;
		glutPostRedisplay();
		return;

	//move forward on W
	case 'w': case 'W': Global::positMat[2][3] += Global::SPEED; glutPostRedisplay(); return;

	//move backward on S
	case 's': case 'S': Global::positMat[2][3] -= Global::SPEED; glutPostRedisplay(); return;

	//move left on A
	case 'a': case 'A': Global::positMat[0][3] += Global::SPEED; glutPostRedisplay(); return;

	//move right on D
	case 'd': case 'D': Global::positMat[0][3] -= Global::SPEED; glutPostRedisplay(); return;

	//crouch on C
	case 'c': case 'C':
		if (Global::crouching) {
			Global::positMat[1][3] -= Global::CROUCH_DIST;
			Global::crouching = false;
		} else {
			Global::positMat[1][3] += Global::CROUCH_DIST;
			Global::crouching = true;
		}
		glutPostRedisplay();
		return;

	//scale Global::positMat on I (zoom in)
	case 'i': case 'I':
		Global::positMat = scale(Global::ZOOMIN) * Global::positMat;
		glutPostRedisplay();
		return;

	//scale down Global::positMat on O (zoom out)
	case 'o': case 'O':
		Global::positMat = scale(Global::ZOOMOUT) * Global::positMat;
		glutPostRedisplay();
		return;

	//Pause and play animation on T
	case 't': case 'T': Global::animating = !Global::animating;

	}
}

void mouse(int button, int state, int p, int q) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		Global::leftMouse = true;
		Global::pRef = p, Global::qRef = q;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		Global::leftMouse = false;
}

void mouseMotion(int p, int q)
//rotates the camera if the left mouse button is pressed down.
//assumes user is clicking and pulling on a plane parallel to the projection plane,
//and approximates the angle of rotation by using the sphere tangent to that plane.
//for the plane at z = -1, that is the unit sphere, along Global::which all arcs are equal
//to the angle that subtends them, meaning that the distance moved by the mouse
//along the tangent plane can be directly plugged in as the angle of rotation
{
	if (Global::leftMouse) {
		float delta_p = 3.0 * Global::FOV * (M_PI/180.0) * (p - Global::pRef) / Global::viewDim;
		float delta_q = 2.5 * Global::FOV * (M_PI/180.0) * (q - Global::qRef) / Global::viewDim;
		mat4 delta_yaw = rY(delta_p);
		Global::pitchTheta =
			(Global::pitchTheta + delta_q > -M_PI/2.0 && Global::pitchTheta + delta_q < M_PI/2.0) ?
				Global::pitchTheta + delta_q
			:	Global::pitchTheta;
		Global::yawMat = delta_yaw * Global::yawMat;
		Global::positMat = delta_yaw * Global::positMat;
		Global::pitchMat = rX(Global::pitchTheta);
		Global::pRef = p, Global::qRef = q;
		glutPostRedisplay();
	}
}

void help() {
	std::cout <<	std::endl <<
					std::endl <<
					"Clicking in the window with the left button\n" <<
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

void init() {
	// Load shaders and use the resulting shader program
    GLuint drawProg = InitShader("Draw.vertex", "Draw.fragment");
	GLuint shadowProg = InitShader("ShadowMap.vertex", "ShadowMap.fragment");
    glUseProgram(drawProg);

	meshSetup(Global::red_opaque, 1.2);

	const mat4 projection = Perspective(Global::FOV, 1.0f, 0.0078125f, 16.0f) * 
		translate(0.0f, 0.0f, 0.75f) * scale(10.0f);

	Global::root = new RenderGraph(
		drawProg,
		shadowProg,
		*genIdyll(5, 1.2f),
		projection
	);
	Global::sun = new RenderGraph(
		drawProg,
		shadowProg,
		*genSun( translate(16.0 * Global::sunVec) * scale(0.4f) *
			rX(M_PI/2.0) ),
		projection
	);

	help();
}

void display() {
	const float sunY = Global::sunVec.y;
	const float tempDirect = pow(abs(sunY), 0.125);
	const float tempBright = (sunY > 0.0) ? 1.0 : 1.0 - pow(abs(sunY), 0.125f);
	glClearColor(	tempBright * 0.6,
					tempBright * (0.25 + 0.35 * tempDirect),
					tempBright * (0.2 + 0.8 * tempDirect),
					1.0	);
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	const mat4 temp = Global::rollMat * Global::pitchMat;
	if (Global::root != nullptr) {
		Global::root->setCamera(temp * Global::positMat);
		Global::root->draw();
	}
	if (Global::sun != nullptr) {
		Global::sun->setCamera(temp * Global::yawMat);
		Global::sun->draw();
	}
	
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
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( Global::viewDim, Global::viewDim );
    glutCreateWindow("Eric Massengill GitHub Graphics Project");
	m_glewInitAndVersion();

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
	//pass mouse function to glut
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	//pass reshape function to glut
	glutReshapeFunc(reshape);
	//pass animation function to glut
	glutTimerFunc(500, timer, 0);

    glutMainLoop();

	return 0;
}