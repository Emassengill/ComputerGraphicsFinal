#version 330 core

uniform mat4 Camera = mat4(1.0);
uniform mat4 vTrans = mat4(1.0);
uniform mat4 nTrans = mat4(1.0);

uniform vec4 sunDirect = {0.4082, 0.8165, 0.4082, 0.0};
uniform vec4 lightPosit = {0.0, 0.0, 0.0, 1.0};
uniform float isInside = 0.0;
uniform float isSun = 0.0;

in vec4 vPosition;
in vec4 vColor;
in vec4 vNormal;

out vec3 point;
out vec3 color;
out vec3 normal;

void main() {
	vec4 tempVec = vTrans * vPosition;
	gl_Position = Camera * tempVec;
	point = tempVec.xyz;
	color = vColor.rgb;
	if (isSun == 0.0) normal = normalize( (nTrans * vNormal).xyz );
	else normal = vec3(0.0);
}