#version 330 core

uniform mat4 Camera = mat4(1.0f);
uniform mat4 Projection = mat4(1.0f);
uniform mat4 Shadows = mat4(1.0f);

uniform mat4 vTrans = mat4(1.0f);
uniform mat4 nSkew = mat4(1.0f);

uniform float specexp = 0.0f;

//uniform uint numLights = 1u;
//uniform sampler2DShadow shadowSampler;

//Vectors for specifying a single directional light source
//and a single point light source.
uniform vec3 directLight = {0.408f, 0.817f, 0.408f};
uniform vec3 pointLight = {0.0f, 0.0f, 0.0f};

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
	vec3 shadow;
	vec3 color;
	vec3 normal;
} vertex;

void main() {
	vec4 tempVec = vTrans * vPosition;
	vertex.global = tempVec.xyz;

	if ( bool(shadowMap) ) { //if shadow mapping
		vertex.shadow = vec3(0.0f);
		vertex.color = vec3(0.0f);
		vertex.normal = vec3(0.0f);
	} else if ( ! bool(isSun) ) { //if not sun and not shadow mapping
		vertex.shadow = (Shadows * tempVec).xyz;
		vertex.color = vColor.rgb;
		vertex.normal = normalize( (nSkew * vNormal).xyz );
	} else { //if sun
		vertex.shadow = vec3(0.0f);
		vertex.color = vColor.rgb;
		vertex.normal = vec3(0.0f);
	}

	tempVec = Camera * tempVec;
	vertex.viewer = tempVec.xyz;
	gl_Position = Projection * tempVec;
}