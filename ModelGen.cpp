#include "ModelGen.h"
#include "LightNode.h"
#include "Lightsource.h"
#include "EnvironmentNode.h"
#include "BoolState.h"

//Model Animations
inline
mat4 rotateSun(float theta) {
	return MatMath::rZ(theta/8.0f);
}

inline
mat4 rotateWheel(float theta) {
	return MatMath::rZ(25 * theta);
}

inline
mat4 driveCar(float theta) {
	return MatMath::rY(theta);
}

//Model Generators
ObjectNode* genSun(const mat4& initTrans) {
	ObjectNode* temp = new EnvironmentNode(BoolState(0u, 1u), initTrans, MatMath::ID, rotateSun);
	temp->addChild(*new ObjectNode(Global::sunDisk));
	return temp;
}

inline
ObjectNode* genGround(float height, float size, const mat4& initTrans, const mat4& initSkew) {
	mat4 tempTrans = MatMath::translate(0.0, -height, 0.0);
	mat4 tempRotate = MatMath::rX(-M_PI/2.0);
	return new ObjectNode(	Global::groundSquare, initTrans * tempTrans * MatMath::scale(size) *
							tempRotate, initSkew * tempRotate	);
}

inline
ObjectNode* genWheel(const mat4& initTrans, const mat4& initSkew) {
	ObjectNode* wheel = new ObjectNode(nullptr, initTrans, initSkew);

	mat4 rotMat = MatMath::rX(M_PI/2.0);
	mat4 tempTransform = MatMath::distort(0.1, 0.1, 0.6);
	wheel->addChild( *new ObjectNode(Global::wheelCylinder, rotMat * tempTransform, rotMat, rotateWheel) );
	mat4 tempRotate = MatMath::rZ(M_PI/4.0);
	wheel->addChild( *new ObjectNode(	Global::wheelCylinder, tempRotate * rotMat * tempTransform,
										tempRotate * rotMat, rotateWheel							) );
	tempRotate = MatMath::rZ(M_PI/2.0);
	wheel->addChild( *new ObjectNode(	Global::wheelCylinder, tempRotate * rotMat * tempTransform,
										tempRotate * rotMat, rotateWheel							) );
	tempRotate = MatMath::rZ(-M_PI/4.0);
	wheel->addChild( *new ObjectNode(	Global::wheelCylinder, tempRotate * rotMat *  tempTransform,
										tempRotate * rotMat, rotateWheel							) );

	wheel->addChild( *new ObjectNode(	Global::wheelTube, MatMath::distort(1.0, 1.0, 0.2), MatMath::ID,
										rotateWheel		) );

	return wheel;
}

inline
ObjectNode* genChassis(const mat4& initTrans, const mat4& initSkew, const color4& paint) {
	ObjectNode* chassis = new ObjectNode(nullptr, initTrans, initSkew);
	
	mat4 tempRotate = MatMath::rX(M_PI/2.0);
	chassis->addChild(	*new ObjectNode(Global::carSphere, MatMath::distort(2.0, 0.5, 1.0) * tempRotate,
						MatMath::distort(0.5, 2.0, 1.0) * tempRotate)	);

	chassis->addChild( *new ObjectNode(	Global::carSphere,
		MatMath::translate(0.3, 0.25, 0.0) * MatMath::distort(1.25, 0.5, 0.85),
		MatMath::distort(0.8, 2.0, 1/0.85))	);
	
	mat4 tempSkew = MatMath::distort(0.35, 0.2, 1.0);
	mat4 tempTrans = MatMath::translate(-2.0, -1.0, 0.0);
	chassis->addChild( *new ObjectNode(Global::carCube, tempSkew * tempTrans) );
	tempTrans[0][3] = 2.85;
	chassis->addChild( *new ObjectNode(Global::carCube, tempSkew * tempTrans) );

	return chassis;
}

inline
ObjectNode* genCar(const mat4& initTrans, const mat4& initSkew, const color4& paint) {
	ObjectNode* car = new ObjectNode(nullptr, initTrans, initSkew);
	car->addChild(*genChassis(MatMath::ID, MatMath::ID, paint));
	
	mat4 tempSkew = MatMath::scale(0.3);
	mat4 tempTrans = MatMath::translate(-0.7, -0.36, 0.75);
	car->addChild(*genWheel(tempTrans * tempSkew));
	tempTrans[0][3] = 1.01;
	car->addChild(*genWheel(tempTrans * tempSkew));
	tempTrans[2][3] = -0.75;
	car->addChild(*genWheel(tempTrans * tempSkew));
	tempTrans[0][3] = -0.7;
	car->addChild(*genWheel(tempTrans * tempSkew));

	return car;
}

inline
ObjectNode* genRoad(const mat4& initTrans, const mat4& initSkew, float thinness) {
	mat4 tempRotate = MatMath::rX(-M_PI/2.0);
	return new ObjectNode(Global::roadRing, initTrans * tempRotate, initSkew * tempRotate);
}

inline
ObjectNode* genDrivingScene(float thinness, const mat4& initTrans, const mat4& initSkew) {
	ObjectNode* road = new ObjectNode(nullptr, initTrans, initSkew);
	road->addChild(*genRoad(MatMath::ID, MatMath::ID, thinness));
	const float width = 1.0 - 0.5 * thinness;
	mat4 tempTrans = MatMath::translate(0.0, 0.1 * width, -0.5 * (thinness + width) + 0.1 * width);
	ObjectNode* temp = new ObjectNode(nullptr, MatMath::ID, MatMath::ID, driveCar);
	temp->addChild(*genCar(tempTrans * MatMath::scale(0.15 * width)));///
	road->addChild(*temp);///
	return road;
}

inline
ObjectNode* genLeaf(float prob, const mat4& initTrans, const mat4& initSkew) {
	const int colorInd = Global::bernoulli() < prob ? 1 : 0;
	return new ObjectNode(	Global::leaf[colorInd], initTrans * MatMath::rZ(M_PI/12.0),
							initSkew * MatMath::rZ(M_PI/12.0)							);
}

inline
ObjectNode* genLeafBunch(float rho, float radius, float prob, int numLeaves,
	const mat4& initTrans, const mat4& initSkew)
{
	ObjectNode* bunch = new ObjectNode(nullptr, initTrans, initSkew);

	mat4 rotMat = MatMath::ID;
	mat4 tempTrans = MatMath::translate(0.0, radius, 0.0);
	mat4 tempRotate = MatMath::rY(	Global::randNum(0.0, 2.0 * M_PI) ) *
									MatMath::rZ(Global::randNum(0.0, rho)	);
	ObjectNode* temp = new ObjectNode(nullptr, tempRotate * tempTrans, tempRotate);
	temp->addChild(*genLeaf(prob));
	bunch->addChild(*temp);
	for (int i = 1; i < numLeaves; ++i) {
		rotMat = MatMath::rY(	Global::randNum(0.0, 2.0 * M_PI) ) *
								MatMath::rX( Global::randNum(0.0, M_PI/2.0)	);
		tempRotate = MatMath::rY(	Global::randNum(0.0, 2.0 * M_PI) ) *
									MatMath::rX( Global::randNum(0.0, rho)	);
		temp = new ObjectNode(nullptr, tempRotate * tempTrans * rotMat, tempRotate * rotMat);
		temp->addChild(*genLeaf(prob));
		bunch->addChild(*temp);
	}

	return bunch;
}

inline
ObjectNode* genBranch(int branching, int maxDepth, int density, float scaleFactor, float seasonProb,
	const mat4& initTrans, const mat4& initSkew, int currentDepth)
{
	mat4 tempTrans = MatMath::distort(0.2, 1.0, 0.2);
	mat4 tempMove = MatMath::translate(0.0, 1.0, 0.0);
	mat4 tempSkew = MatMath::distort(5.0, 1.0, 5.0);
	mat4 tempRotate = MatMath::rX(M_PI/2.0);
	ObjectNode* branch = new ObjectNode(nullptr, initTrans, initSkew);
	branch->addChild(*new ObjectNode(	Global::treeCone, tempMove * tempTrans * tempRotate,
										tempSkew * tempRotate )		);

	if (currentDepth < maxDepth) {
		const int branchFact = branching;
		mat4 tempRotate = MatMath::rZ(M_PI/6.0);
		mat4 rotMat = MatMath::rY(2.0 * M_PI / branchFact);
		tempTrans = MatMath::scale(scaleFactor);
		tempMove = MatMath::translate(0.0, 1.5, 0.0);
		mat4 twist = MatMath::rY(M_PI / branchFact) * tempMove;
		mat4 loopTemp = rotMat * tempRotate;

		for (int i = 0; i < branchFact; ++i) {
			loopTemp = rotMat * loopTemp;
			branch->addChild( *genBranch( branching, maxDepth, density, scaleFactor, seasonProb,
				twist * loopTemp * tempTrans, loopTemp, currentDepth + 1 ) );
		}
	}
	
	return branch;
}

inline
ObjectNode* genTree(int branching, int maxDepth, int density, float scaleFactor, float seasonProb, float height,
	const mat4& initTrans, const mat4& initSkew)
{
	ObjectNode* tree = new ObjectNode(nullptr,initTrans, initSkew);

	mat4 tempRotate = MatMath::rX(M_PI/2.0);
	mat4 tempTrans = MatMath::distort(1.0, 0.2, 1.0);
	mat4 tempSkew = MatMath::distort(1.0, 5.0, 1.0);
	mat4 tempMove = MatMath::translate(0.0, -2.0, 0.0);
	tree->addChild( *new ObjectNode(	Global::treeCone, tempMove * tempTrans * tempRotate,
									tempSkew * tempRotate)	);

	tempTrans = MatMath::distort(0.3, height, 0.3);
	tempMove[1][3] = height - 2.0;
	tree->addChild( *new ObjectNode(Global::treeCylinder, tempMove * tempTrans * tempRotate, tempRotate) );

	tempMove[1][3] = height * 0.6 + 0.1;
	tree->addChild( *genBranch(branching, maxDepth, density, scaleFactor, seasonProb, tempMove) );

	mat4 rotMat = MatMath::rY(2.0 * M_PI/4.0);
	tempSkew = MatMath::rZ(M_PI/4.0);
	for (int i = 0; i < 4; ++i) {
		tempSkew = rotMat * tempSkew;
		tree->addChild( *genBranch(branching, maxDepth, density, scaleFactor, seasonProb,
			tempMove * tempSkew, tempSkew) );
	}

	tree->addChild( *genLeafBunch(M_PI/2.0, 3.0, 0.5, density,
		MatMath::translate(0.0, height, 0.0) * MatMath::scale(0.7)) );

	return tree;
}

inline
ObjectNode* genForest(int numTrees, float minR, float maxR, const mat4& initTrans, const mat4& initSkew) {
	ObjectNode* forest = new ObjectNode(nullptr, initTrans, initSkew);

	mat4 rotMat = MatMath::rY(0.3 * numTrees + 2.0 * M_PI / numTrees);
	mat4 tempPosit = MatMath::ID;
	for (int i = 0; i < numTrees; ++i, rotMat *= rotMat) {
		tempPosit = rotMat * MatMath::translate(0.0, 0.0, -Global::randNum(minR, maxR));
		forest->addChild( *genTree( 3, 2, 150, 0.5, 0.5, 1.6, tempPosit, tempPosit ) );
	}

	return forest;
}

inline
ObjectNode* genFirePlace(const mat4& initTrans, const mat4& initSkew) {
	return new ObjectNode(Global::furnTube, initTrans * MatMath::rZ(M_PI/4.0), initSkew * MatMath::rZ(M_PI/4.0));
}

inline
ObjectNode* genBed(const mat4& initTrans, const mat4& initSkew) {
	ObjectNode* bed = new ObjectNode(nullptr, initTrans, initSkew);

	bed->addChild( *new ObjectNode(Global::furnCube0, MatMath::translate(0.5, 0.0, 0.0) * MatMath::distort(0.3, 0.2, 0.4)) );
	bed->addChild( *new ObjectNode(Global::furnCube1, MatMath::translate(-0.5, 0.0, 0.0) * MatMath::distort(0.7, 0.2, 0.4)) );
	bed->addChild( *new ObjectNode(	Global::treeCylinder,
		MatMath::translate(-1.1, -0.3, 0.3) * MatMath::scale(0.1) * MatMath::rX(M_PI/2.0),
		MatMath::rX(M_PI/2.0)) );
	bed->addChild( *new ObjectNode(	Global::treeCylinder, MatMath::translate(0.7, -0.3, 0.3) *
		MatMath::scale(0.1) * MatMath::rX(M_PI/2.0), MatMath::rX(M_PI/2.0))	);
	bed->addChild( *new ObjectNode(	Global::treeCylinder, MatMath::translate(0.7, -0.3, -0.3) *
		MatMath::scale(0.1) * MatMath::rX(M_PI/2.0), MatMath::rX(M_PI/2.0))	);
	bed->addChild( *new ObjectNode(	Global::treeCylinder, MatMath::translate(-1.1, -0.3, -0.3) *
		MatMath::scale(0.1) * MatMath::rX(M_PI/2.0), MatMath::rX(M_PI/2.0))	);

	return bed;
}

inline
ObjectNode* genChair(const mat4& initTrans, const mat4& initSkew) {
	ObjectNode* chair = new ObjectNode(nullptr, initTrans, initSkew);

	chair->addChild( *new ObjectNode(Global::furnCube1, MatMath::distort(0.5, 0.2, 0.4)) );
	chair->addChild( *new ObjectNode(	Global::furnCube1, MatMath::translate(-0.5, 0.5, 0.0) *
		MatMath::rZ(M_PI/8.0) * MatMath::distort(0.15, 0.45, 0.4), MatMath::rZ(M_PI/8.0))		);
	chair->addChild( *new ObjectNode(	Global::furnCube1, MatMath::translate(0.0, 0.3, 0.45) *
										MatMath::distort(0.4, 0.1, 0.1))						);
	chair->addChild( *new ObjectNode(	Global::furnCube1, MatMath::translate(0.0, 0.3, -0.45) *
										MatMath::distort(0.4, 0.1, 0.1))						);
	chair->addChild( *new ObjectNode(	Global::treeCylinder, MatMath::translate(-0.4, -0.3, 0.3) *
		MatMath::scale(0.1) * MatMath::rX(M_PI/2.0), MatMath::rX(M_PI/2.0))		);
	chair->addChild( *new ObjectNode(	Global::treeCylinder, MatMath::translate(0.4, -0.3, 0.3) *
		MatMath::scale(0.1) * MatMath::rX(M_PI/2.0), MatMath::rX(M_PI/2.0))	);
	chair->addChild( *new ObjectNode(	Global::treeCylinder, MatMath::translate(0.4, -0.3, -0.3) *
		MatMath::scale(0.1) * MatMath::rX(M_PI/2.0), MatMath::rX(M_PI/2.0))	);
	chair->addChild( *new ObjectNode(	Global::treeCylinder, MatMath::translate(-0.4, -0.3, -0.3) *
		MatMath::scale(0.1) * MatMath::rX(M_PI/2.0), MatMath::rX(M_PI/2.0))	);

	return chair;
}

inline
ObjectNode* genHouseOut(const mat4& initTrans, const mat4& initSkew) {
	ObjectNode* house = new ObjectNode(nullptr, initTrans, initSkew);

	const float height = 0.5;
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(0.0, -height, 0.0) *
										MatMath::distort(1.0, 0.01, 1.0)						) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(0.0, 0.0, -1.0) *
										MatMath::distort(1.0, height, 0.01)						) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(1.0, 0.0, 0.0) *
										MatMath::distort(0.01, height, 1.0)						) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(-1.0, 0.0, 0.0) *
										MatMath::distort(0.01, height, 1.0)						) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(0.58, 0.0, 1.0) *
										MatMath::distort(0.42, height, 0.01)					) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(-0.58, 0.0, 1.0) *
										MatMath::distort(0.42, height, 0.01)					) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(0.0, 0.35, 1.0) *
										MatMath::distort(0.25, 0.3 * height, 0.01)				) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(-0.29, -0.16, 1.13) *
		MatMath::rY(M_PI/4.0) * MatMath::distort(0.2, 0.7 * height, 0.01), MatMath::rY(M_PI/4.0)	) );
	house->addChild( *new ObjectNode(	Global::housePyramid,
		MatMath::translate(0.0, 0.85, 0.0) * MatMath::scale(1.7) * MatMath::distort(1.0, 0.2, 1.0) *
		MatMath::rY(M_PI/4.0) * MatMath::rX(M_PI/2.0),
		MatMath::distort(1.0, 5.0, 1.0) * MatMath::rY(M_PI/4.0) * MatMath::rX(M_PI/2.0)				) );

	return house;
}

inline
ObjectNode* genHouseIn(const mat4& initTrans, const mat4& initSkew) {
	ObjectNode* house = new EnvironmentNode(BoolState(1u, 0u), initTrans, initSkew);
	house->addChild(
		*new LightNode(Lightsource("pointLight", false), MatMath::translate(0.0, -0.35, -0.87))		);

	const float height = 0.5;
	house->addChild( *new ObjectNode(Global::houseSquare, MatMath::translate(0.0, -height + 0.02, 0.0) *
									MatMath::rX(M_PI/2.0), MatMath::rX(M_PI/2.0)					) );
	house->addChild( *new ObjectNode(Global::houseSquare, MatMath::translate(0.0, height - 0.02, 0.0) *
									MatMath::rX(M_PI/2.0), MatMath::rX(M_PI/2.0)					) );
	house->addChild( *new ObjectNode(	Global::houseSquare, MatMath::translate(0.0, 0.0, -0.98) *
										MatMath::distort(1.0, height, 1.0)							) );
	house->addChild( *new ObjectNode(	Global::houseSquare, MatMath::translate(0.98, 0.0, 0.0) *
		MatMath::distort(1.0, height, 1.0) * MatMath::rY(M_PI/2.0), MatMath::rY(M_PI/2.0)			) );
	house->addChild( *new ObjectNode(	Global::houseSquare, MatMath::translate(-0.98, 0.0, 0.0) *
		MatMath::distort(1.0, height, 1.0) * MatMath::rY(M_PI/2.0), MatMath::rY(M_PI/2.0)			) );
	house->addChild( *new ObjectNode(	Global::houseSquare, MatMath::translate(0.58, 0.0, 0.98) *
										MatMath::distort(0.42, height, 1.0)							) );
	house->addChild( *new ObjectNode(	Global::houseSquare, MatMath::translate(-0.58, 0.0, 0.98) *
										MatMath::distort(0.42, height, 1.0)							) );
	house->addChild( *new ObjectNode(	Global::houseSquare, MatMath::translate(0.0, 0.35, 0.98) *
										MatMath::distort(0.25, 0.3 * height, 1.0)					) );

	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(-0.2, 0.0, 0.6) *
										MatMath::distort(0.01, height, 0.4)						) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(0.2, 0.0, 0.6) *
										MatMath::distort(0.01, height, 0.4)						) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(-0.75, 0.0, 0.2) *
										MatMath::distort(0.25, height, 0.01)					) );
	house->addChild( *new ObjectNode(	Global::houseCube, MatMath::translate(0.75, 0.0, 0.2) *
										MatMath::distort(0.25, height, 0.01)					) );

	house->addChild( *genFirePlace( MatMath::translate(0.0, -0.35, -0.95) *
		MatMath::distort(1.5, 1.25, 0.2) * MatMath::scale(0.25) ));
	house->addChild( *genBed(MatMath::translate(-0.5, -0.37, 0.85) * MatMath::scale(0.3)) );
	house->addChild( *genBed(MatMath::translate(0.65, -0.37, 0.85) * MatMath::scale(0.3)) );
	house->addChild( *genChair(MatMath::translate(-0.3, -0.4, -0.35) *
		MatMath::rY(M_PI/4.0) * MatMath::scale(0.2), MatMath::rY(M_PI/4.0)) );
	house->addChild( *genChair(MatMath::translate(0.25, -0.4, -0.3) *
		MatMath::rY(M_PI/2.0) * MatMath::scale(0.2), MatMath::rY(M_PI/2.0)) );
	house->addChild( *genChair(MatMath::translate(0.8, -0.4, -0.4) *
		MatMath::rY(M_PI) * MatMath::distort(1.0, 1.0, 3.0) * MatMath::scale(0.2), MatMath::rY(M_PI)) );

	return house;
}

inline
ObjectNode* genHouse(const mat4& initTrans, const mat4& initSkew) {
	ObjectNode* house = new ObjectNode(nullptr, initTrans, initSkew);
	house->addChild(*genHouseOut());
	house->addChild(*genHouseIn());
	return house;
}

ObjectNode* genIdyll(int numRows, float thinness, const mat4& initTrans, const mat4& initSkew) {
	ObjectNode* scene = new EnvironmentNode(BoolState(0u, 0u), initTrans, initSkew);

	scene->addChild( *new LightNode(Lightsource("directLight", true), MatMath::ID, rotateSun) );
	scene->addChild( *genDrivingScene(thinness) );
	scene->addChild( *genGround(0.001, 20.0) );
	mat4 tempRotate = MatMath::ID;
	for (int i = 0, numTrees = 5; i < numRows; ++i, numTrees += 5) {
		scene->addChild( *genForest(numTrees, 11.0 + i * 10.0, 8.0 + (i + 1) * 10.0,
		MatMath::translate(0.0, 0.32, 0.0) * MatMath::scale(0.15) ));
	}
	scene->addChild( *genHouse(MatMath::translate(0.0, 0.1, 0.0) * MatMath::scale(0.2)) );

	return scene;
}