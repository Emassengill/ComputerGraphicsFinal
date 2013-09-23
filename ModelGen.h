#ifndef __MODEL_GEN__
#define __MODEL_GEN__

#include "Global.h"
#include "MatMath.h"

class ObjectNode;
class EnvironmentNode;

//Model Animations
mat4 rotateWheel(float theta);
mat4 driveCar(float theta);

//Model Generators
EnvironmentNode* genSun(const mat4& initTrans);
ObjectNode* genGround(float height = 0.0, float size = 1.0,
	const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genWheel(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genChassis(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID,
	const color4& paint = Global::red_opaque);
ObjectNode* genCar(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID,
	const color4& paint = Global::red_opaque);
ObjectNode* genRoad(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID,
	float thinness = 1.0);
ObjectNode* genDrivingScene(float thinness = 1.0, const mat4& initTrans = MatMath::ID,
	const mat4& initSkew = MatMath::ID);
ObjectNode* genLeaf(float prob = 0.5, const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genLeafBunch(float rho = M_PI/3.0, float radius = 1.0, float prob = 0.5,
				int numLeaves = 10, const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genBranch(int branching = 3, int maxDepth = 4, int density = 10, float scaleFactor = 0.6,
	float seasonProb = 0.5, const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID,
	int currentDepth = 0);
ObjectNode* genTree(int branching = 3, int maxDepth = 2, int density = 15, float scaleFactor = 0.6,
	float seasonProb = 0.15, float height = 1.6,
	const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genForest(int numTrees = 10, float minR = 5.0, float maxR = 15.0,
	const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genFirePlace(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genBed(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genChair(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genHouseOut(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
EnvironmentNode* genHouseIn(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
ObjectNode* genHouse(const mat4& initTrans = MatMath::ID, const mat4& initSkew = MatMath::ID);
EnvironmentNode* genIdyll(int numRows = 2, float thinness = 1.0, const mat4& initTrans = MatMath::ID,
	const mat4& initSkew = MatMath::ID);

#endif