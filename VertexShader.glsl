#version 330 core

uniform mat4 Camera = mat4(1.0f);
uniform mat4 Projection = mat4(1.0f);

uniform mat4 vTrans = mat4(1.0f);
uniform mat4 nTrans = mat4(1.0f);

uniform float specexp = 0.0f;

//Vectors for specifying a single directional light source
//and a single point light source.
uniform vec4 directLight = {0.408f, 0.817f, 0.408f, 0.0f};
uniform vec3 pointLight = {0.0f, 0.0f, 0.0f};

//These uniform floats are used as boolean values that specify
//functionality in the shaders. In this way, shading models can
//be switched without having to change the entire shading program.
uniform float isInside = 0.0f;
uniform float isSun = 0.0f;
uniform float shadowMap = 0.0f;

in vec4 vPosition;
in vec4 vColor;
in vec4 vNormal;

out vec3 globalPoint;
out vec3 viewerPoint;
out vec3 color;
out vec3 normal;

void main() {
	vec4 tempVec = vTrans * vPosition;
	globalPoint = tempVec.xyz;
	tempVec = Camera * tempVec;
	viewerPoint = tempVec.xyz;
	gl_Position = Projection * tempVec;
	if (shadowMap < 0.5f) {
		color = vColor.rgb;
		if (isSun > 0.0f) normal = vec3(0.0f);
		else normal = normalize( (nTrans * vNormal).xyz );
	} else {
		color = vec3(0.0f);
		normal = vec3(0.0f);
	}
}