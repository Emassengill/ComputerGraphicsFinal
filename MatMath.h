#ifndef __MAT_MATH__
#define __MAT_MATH__

#include "Definitions.h"

class MatMath {
public:

	static const mat4 ID;
	static const point4 ORIGIN;
	static const normal4 xNORM, yNORM, zNORM;
	static const vec3 xAXIS, yAXIS, zAXIS;
	static const vec3 AXES[3];

};

//Functions
//Matrix math functions

inline
int getMin(float first, float second, float third) {
	if (first <= second) {
		if (first <= third) return 0; //first <= second, third
		else return 2; //third < first  <= second
	} else /*second < first*/ {
		if (second <= third) return 1; //second <= first, third
		else return 2; //third < second < first
	}
}

inline
int getMax(float first, float second, float third) {
	if (first >= second) {
		if (first >= third) return 0; //first >= second, third
		else return 2; // third > first >= second
	} else /*second > first*/ {
		if (second >= third) return 1; // second >= first, third
		else return 2; // third > second > first
	}
}

//rotation matrix generators

inline
mat4 rX(float angle) {
	const float cosine = cos(angle);
	const float sine = sin(angle);
	return mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, cosine, sine, 0.0,
		0.0, -sine, cosine, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}

inline
mat4 rY(float angle) {
	const float cosine = cos(angle);
	const float sine = sin(angle);
	return mat4(
		cosine, 0.0, -sine, 0.0,
		0.0, 1.0, 0.0, 0.0,
		sine, 0.0, cosine, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}

inline
mat4 rZ(float angle) {
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

inline
mat4 rO(float theta, float dx, float dy, float dz) {
	const vec3 v3 = normalize( vec3(dx, dy, dz) );
	
	const int chosenAxis = getMin(
		abs(dot(v3, MatMath::AXES[0])),
		abs(dot(v3, MatMath::AXES[1])),
		abs(dot(v3, MatMath::AXES[2]))
	);

	const vec3 v2 = normalize( cross(v3, MatMath::AXES[chosenAxis]) );
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
inline
mat4 rO(float theta, const vec3& axis) {
	const vec3 v3 = normalize(axis);
	
	const int chosenAxis = getMin(
		abs(dot(v3, MatMath::AXES[0])),
		abs(dot(v3, MatMath::AXES[1])),
		abs(dot(v3, MatMath::AXES[2]))
	);

	const vec3 v2 = normalize( cross(v3, MatMath::AXES[chosenAxis]) );
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

inline
mat4 scale(float factor) {
	return mat4(
		factor, 0.0, 0.0, 0.0,
		0.0, factor, 0.0, 0.0,
		0.0, 0.0, factor, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
}

inline
mat4 translate(float x, float y, float z) {
	return mat4(	1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					x, y, z, 1.0		);
}
inline
mat4 translate(const vec3& direction) {
	return mat4(	1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
			direction.x, direction.y, direction.z, 1.0 );
}
inline
mat4 translate(const vec4& direction) {
	return mat4(	1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
			direction.x, direction.y, direction.z, 1.0 );
}

inline
mat4 distort(float x, float y, float z) {
	return mat4(	x, 0.0, 0.0, 0.0,
					0.0, y, 0.0, 0.0,
					0.0, 0.0, z, 0.0,
					0.0, 0.0, 0.0, 1.0	);
}

inline
vec4 genTranslation4(const mat4& matrix) {
	return vec4(matrix[0][3], matrix[1][3], matrix[2][3], 1.0f);
}
inline
vec3 genTranslation3(const mat4& matrix) {
	return vec3(matrix[0][3], matrix[1][3], matrix[2][3]);
}

#endif