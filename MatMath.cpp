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