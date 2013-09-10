#include "Global.h"

//PUBLIC

// RGBA color palette
const color4 Global::white_opaque( 1.0, 1.0, 1.0, 1.0 );
const color4 Global::black_opaque( 0.0, 0.0, 0.0, 1.0 );
const color4 Global::steel_opaque( 0.75, 0.75, 0.75, 1.0 );
const color4 Global::red_opaque( 1.0, 0.0, 0.0, 1.0 );
const color4 Global::darkred_opaque( 0.7, 0.1, 0.0, 1.0 );
const color4 Global::darkgreen_opaque( 0.2, 0.65, 0.2, 1.0 );
const color4 Global::blue_opaque( 0.0, 0.0, 1.0, 1.0 );
const color4 Global::yellow_opaque( 1.0, 1.0, 0.0, 1.0 );
const color4 Global::darkbrown_opaque( 0.6, 0.4, 0.2, 1.0 );

const float Global::SPEED = 1.5;
bool Global::crouching = false;
bool Global::animating = true;

const float Global::ZOOMIN = 1.2;
const float Global::ZOOMOUT = 1.0/ZOOMIN;

vec4 Global::sunVec(0.0, 1.0, 0.0, 0.0);

//tree
RenderGraph* Global::root, * Global::sun;

//camera matrices
mat4 Global::positMat(1.0);
mat4 Global::pitchMat(1.0);
mat4 Global::rollMat(1.0);
mat4 Global::yawMat(1.0);

float Global::pitchTheta = 0.0;
const float Global::FOV = 60.0;

//window width and height
int Global::ww = 512;
int Global::wh = 512;
//viewport (x, y) coordinate, and viewport width-height (same size)
int Global::viewx = 0;
int Global::viewy = 0;
int Global::viewDim = 512;

//mouse callback values
bool Global::leftMouse = false;
int Global::pRef = 0, Global::qRef = 0;

//Global Objects
//For Chassis
Object* Global::carSphere, * Global::carCube;
//For Wheel
Object* Global::wheelTube, * Global::wheelCylinder;
//For Road
Object* Global::roadRing;
//For Ground
Object* Global::groundSquare;
//For Leaf
Object* Global::leaf[2];
//For Tree
Object* Global::treeCone, * Global::treeCylinder;
//For Sun
Object* Global::sunDisk;
//For House
Object* Global::houseCube, * Global::housePyramid, * Global::houseSquare;
//For Furniture
Object* Global::furnTube, * Global::furnCube0, * Global::furnCube1;

//shader pointers
/*GLuint Global::currentProg;
/*GLuint Global::camera_loc, Global::proj_loc, Global::trans_loc, Global::skew_loc;
GLuint Global::spec_loc, Global::sun_loc;
GLuint Global::isSun_loc, Global::isInside_loc, Global::shadowMap_loc;
GLuint Global::shadowTex;*/
//GLuint Global::shadowBuffer;

//coordinate system conversion functions
inline
int Global::coord2screen(float x) {
	return (int) ((x + 1.0)/2.0) * Global::viewDim;
}

inline
float Global::screen2coord(int x) {
	return  2.0 * x / Global::viewDim - 1.0;
}

inline
int Global::round(float x) { return (int)floor(x + 0.5); }

float Global::randNum(float start, float end) {
	return (end - start) * rand() / RAND_MAX + start;
}

float Global::bernoulli() { return (float) rand() / (RAND_MAX + 1); }