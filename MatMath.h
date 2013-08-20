#ifndef __MAT_MATH__
#define __MAT_MATH__

#include "Definitions.h"

class MatMath {
public:

	static const mat4 ID;
	static const point4 ORIGIN;
	static const normal4 xNORM;
	static const normal4 yNORM;
	static const normal4 zNORM;
	static const vec3 AXES[3];

	//Functions

	static int getMin(float first, float second, float third);
	static int getMax(float first, float second, float third);
	//rotation matrix generators
	static mat4 rX(float angle);
	static mat4 rY(float angle);
	static mat4 rZ(float angle);
	//rotation matrix for an arbitrary axis
	static mat4 rO(float theta, float dx, float dy,  float dz);
	static mat4 rO(float theta, const vec3& axis);
	//transformation matrix generators
	static mat4 scale(float factor);
	static mat4 translate(float x, float y, float z);
	static mat4 distort(float x, float y,float z);

};

#endif