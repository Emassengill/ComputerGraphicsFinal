#version 330 core

const uint MAX_LIGHTS = 1u;
uniform uint numLights = 1u;

uniform mat4 Camera = mat4(1.0f);
uniform mat4 Projection = mat4(1.0f);
uniform mat4 Shadowers[MAX_LIGHTS];

uniform mat4 vTrans = mat4(1.0f);
uniform mat4 nSkew = mat4(1.0f);

//These uniform ints are used as boolean values that specify
//functionality in the shaders. In this way, shading models can
//be switched without having to change the entire shading program.
uniform uint isInside = 0u;
uniform uint isSun = 0u;
uniform uint shadowMap = 0u;

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec4 vNormal;

out Interface {
	vec3 global;
	vec3 viewer;
	vec4 shadows[MAX_LIGHTS];
	vec3 color;
	vec3 normal;
} vertex;

void main() {
	vec4 tempVec = vTrans * vPosition;
	vertex.global = tempVec.xyz;

	if ( bool(shadowMap) ) { //if shadow mapping
		for (uint i = 0u; i < MAX_LIGHTS; ++i) vertex.shadows[i] = vec4(0.0f);
		vertex.color = vec3(0.0f);
		vertex.normal = vec3(0.0f);
	} else if ( ! bool(isSun) ) { //if not sun and not shadow mapping
		for (uint i = 0u; i < numLights; ++i) vertex.shadows[i] = Shadowers[i] * tempVec;
		for (uint i = numLights; i < MAX_LIGHTS; ++i) vertex.shadows[i] = vec4(0.0f);
		vertex.color = vColor.rgb;
		vertex.normal = normalize( (nSkew * vNormal).xyz );
	} else { //if sun
		for (uint i = 0u; i < MAX_LIGHTS; ++i) vertex.shadows[i] = vec4(0.0f);
		vertex.color = vColor.rgb;
		vertex.normal = vec3(0.0f);
	}

	tempVec = Camera * tempVec;
	vertex.viewer = tempVec.xyz;
	gl_Position = Projection * tempVec;
}