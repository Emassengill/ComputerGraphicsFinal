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
	static bool crouching;
	static bool animating;

	static const float ZOOMIN;
	static const float ZOOMOUT;

	static vec4 sunVec;
	
	//tree
	static RenderGraph* root, * sun;

	//camera matrices
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
	static const int SHADOW_BUFFER_DIM = 1024;

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

	//shader pointers
	/*static GLuint currentProg;
	/*static GLuint camera_loc, proj_loc, trans_loc, skew_loc;
	static GLuint spec_loc, sun_loc, isSun_loc, isInside_loc, shadowMap_loc;
	static GLuint shadowTex;*/
	//static GLuint shadowBuffer;

	//Functions
	//coordinate system conversion functions
	static int coord2screen(float x);
	static float screen2coord(int x);
	static int round(float x);
	static float randNum(float start, float end);
	static float bernoulli();

};

#endif