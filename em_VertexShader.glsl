#version 330 core


uniform mat4 Camera = mat4(1.0f);
uniform mat4 vTrans = mat4(1.0f);
uniform mat4 nTrans = mat4(1.0f);

//Vectors for specifying a single directional light source
//and a single point light source.
uniform vec4 sunDirect = {0.4082f, 0.8165f, 0.4082f, 0.0f};
uniform vec4 lightPosit = {0.0f, 0.0f, 0.0f, 1.0f};

//These uniform floats are used as boolean values that specify
//functionality in the shaders. In this way, shading models can
//be switched without having to change the entire shading program.
uniform float isInside = 0.0f;
uniform float isSun = 0.0f;
uniform float occlusionTest = 0.0f;

in vec4 vPosition;
in vec4 vColor;
in vec4 vNormal;

out vec3 point;
out vec3 color;
out vec3 normal;

float gl_ClipDistance[1];

void main() {
	vec4 tempVec = vTrans * vPosition;
	vec4 position = Camera * tempVec;
	gl_ClipDistance[0] = 1.0 + position.z;
	gl_Position = position;
	point = tempVec.xyz;
	color = vColor.rgb;
	if (occlusionTest == 1.0f || isSun == 1.0f) normal = vec3(0.0f);
	else normal = normalize( (nTrans * vNormal).xyz );
}