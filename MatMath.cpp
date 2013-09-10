#include "MatMath.h"

//PUBLIC

const mat4 MatMath::ID(1.0);
const point4 MatMath::ORIGIN(0.0, 0.0, 0.0, 1.0);
const normal4 MatMath::xNORM(1.0, 0.0, 0.0, 0.0);
const normal4 MatMath::yNORM(0.0, 1.0, 0.0, 0.0);
const normal4 MatMath::zNORM(0.0, 0.0, 1.0, 0.0);
const vec3 MatMath::xAXIS(1.0, 0.0, 0.0);
const vec3 MatMath::yAXIS(0.0, 1.0, 0.0);
const vec3 MatMath::zAXIS(0.0, 0.0, 1.0);
const vec3 MatMath::AXES[3] = { xAXIS, yAXIS, zAXIS };

inline
int MatMath::getMin(float first, float second, float third) {
	if (first <= second) {
		if (first <= third) return 0; //first <= second, third
		else return 2; //third < first  <= second
	} else /*second < first*/ {
		if (second <= third) return 1; //second <= first, third
		else return 2; //third < second < first
	}
}

inline
int MatMath::getMax(float first, float second, float third) {
	if (first >= second) {
		if (first >= third) return 0; //first >= second, third
		else return 2; // third > first >= second
	} else /*second > first*/ {
		if (second >= third) return 1; // second >= first, third
		else return 2; // third > second > first
	}
}

//rotation matrix generators

mat4 MatMath::rX(float angle) {
	const float cosine = cos(angle);
	const float sine = sin(angle);
	return mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, cosine, sine, 0.0,
		0.0, -sine, cosine, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}

mat4 MatMath::rY(float angle) {
	const float cosine = cos(angle);
	const float sine = sin(angle);
	return mat4(
		cosine, 0.0, -sine, 0.0,
		0.0, 1.0, 0.0, 0.0,
		sine, 0.0, cosine, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}

mat4 MatMath::rZ(float angle) {
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

mat4 MatMath::rO(float theta, float dx, float dy, float dz) {
	const vec3 v3 = normalize( vec3(dx, dy, dz) );
	
	const int chosenAxis = getMin(
		abs(dot(v3, AXES[0])),
		abs(dot(v3, AXES[1])),
		abs(dot(v3, AXES[2]))
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

mat4 MatMath::rO(float theta, const vec3& axis) {
	const vec3 v3 = normalize(axis);
	
	const int chosenAxis = getMin(
		abs(dot(v3, AXES[0])),
		abs(dot(v3, AXES[1])),
		abs(dot(v3, AXES[2]))
		);

	const vec3 v2 = normalize( cross(v3, AXES[chosenAxis]) );
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

mat4 MatMath::scale(float factor) {
	return mat4(
		factor, 0.0, 0.0, 0.0,
		0.0, factor, 0.0, 0.0,
		0.0, 0.0, factor, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}

mat4 MatMath::translate(float x, float y, float z) {
	return mat4(	1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					x, y, z, 1.0		);
}
mat4 MatMath::translate(const vec3& direction) {
	return mat4(	1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
			direction.x, direction.y, direction.z, 1.0 );
}
mat4 MatMath::translate(const vec4& direction) {
	return mat4(	1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
			direction.x, direction.y, direction.z, 1.0 );
}

mat4 MatMath::distort(float x, float y, float z) {
	return mat4(	x, 0.0, 0.0, 0.0,
					0.0, y, 0.0, 0.0,
					0.0, 0.0, z, 0.0,
					0.0, 0.0, 0.0, 1.0	);
}