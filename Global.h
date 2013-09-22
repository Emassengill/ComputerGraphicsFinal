#ifndef __GLOBAL__
#define __GLOBAL__

#include "Definitions.h"

class Node;
class Object;
class RenderGraph;

class Global {
public:

	// RGBA color palette
	static const color4 white_opaque;
	static const color4 black_opaque;
	static const color4 steel_opaque;
	static const color4 red_opaque;
	static const color4 darkred_opaque;
	static const color4 darkgreen_opaque;
	static const color4 blue_opaque;
	static const color4 yellow_opaque;
	static const color4 darkbrown_opaque;

	static const float SPEED;
	static const float CROUCH_DIST;
	static bool crouching;
	static bool animating;

	static const float ZOOMIN;
	static const float ZOOMOUT;

	static vec4 sunVec;
	
	//tree
	static RenderGraph* root, * sun;

	//camera matrices
	static const mat4 initPosit;
	static mat4 positMat;
	static mat4 pitchMat;
	static mat4 rollMat;
	static mat4 yawMat;

	static float pitchTheta;
	static const float FOV;

	//window width and height
	static int ww;
	static int wh;
	//viewport (x, y) coordinate, and viewport width-height (same size)
	static int viewx;
	static int viewy;
	static int viewDim;
	static const int SHADOW_BUFFER_DIM = GL_MAX_VIEWPORT_DIMS;

	//mouse callback values
	static bool leftMouse;
	static int pRef, qRef;

	//Global Objects
	//For Chassis
	static Object* carSphere, * carCube;
	//For Wheel
	static Object* wheelTube, * wheelCylinder;
	//For Road
	static Object* roadRing;
	//For Ground
	static Object* groundSquare;
	//For Leaf
	static Object* leaf[2];
	//For Tree
	static Object* treeCone, * treeCylinder;
	//For Sun
	static Object* sunDisk;
	//For House
	static Object* houseCube, * housePyramid, * houseSquare;
	//For Furniture
	static Object* furnTube, * furnCube0, * furnCube1;

	static void cleanup();

private:
	Global() { }
	Global(const Global& that) { }
	void operator = (const Global& that) { }

};

//Functions
//coordinate system conversion functions
inline
int coord2screen(float x) {
	return (int) ((x + 1.0)/2.0) * Global::viewDim;
}

inline
float screen2coord(int x) {
	return  2.0 * x / Global::viewDim - 1.0;
}

inline
int round(float x) { return (int)floor(x + 0.5); }

inline
float randNum(float start, float end) {
	return (end - start) * rand() / RAND_MAX + start;
}

inline
float bernoulli() { return (float) rand() / (RAND_MAX + 1); }

//PUBLIC: Global

inline
void Global::cleanup() {
	if (root != nullptr) delete root;
	if (sun != nullptr) delete sun;

	//Global Objects
	//For Chassis
	delete carSphere; delete carCube;
	//For Wheel
	delete wheelTube; delete wheelCylinder;
	//For Road
	delete roadRing;
	//For Ground
	delete groundSquare;
	//For Leaf
	delete leaf[0]; delete leaf[1];
	//For Tree
	delete treeCone; delete treeCylinder;
	//For Sun
	delete sunDisk;
	//For House
	delete houseCube; delete housePyramid; delete houseSquare;
	//For Furniture
	delete furnTube; delete furnCube0; delete furnCube1;
}

#endif