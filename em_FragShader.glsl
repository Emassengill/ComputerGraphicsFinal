#version 330 core

uniform vec4 sunDirect = {0.4082, 0.8165, 0.4082, 0.0};
uniform vec4 lightPosit = {0.0, 0.0, 0.0, 1.0};
uniform float isInside = 0.0;
uniform float isSun = 0.0;

in vec3 point;
in vec3 color;
in vec3 normal;

out vec3 outputColor;

void main() {
	outputColor = color;

	if (isSun == 0.0) {

		float tempDirect = pow(abs(sunDirect.y), 0.4);
		float ambient = 0.536 * pow(sunDirect.y + 1.0, 0.9);

		float modulator;
		vec3 lightColor;

		if (isInside == 1.0) {
			vec3 lightVec = lightPosit.xyz - point;
			float brightness = 54.0 * dot(normal, normalize(lightVec));
			if (brightness <= 0.0) brightness = 0.0;
			else brightness /= 0.1 + pow(length(lightVec), 2.0);
		
			ambient = 0.2 + 0.3 * ambient;
			modulator = brightness + ambient;
			lightColor = vec3( 1.0, 0.65 + 0.35 * tempDirect, 0.4 + 0.6 * tempDirect );
		} else {
			float brightness = dot(normal, sunDirect.xyz);
			if (brightness <= 0.0) brightness = 0.0;
			else brightness *= 0.25 * pow(sunDirect.y + 1.0, 2.0);
		
			ambient = 0.15 + 0.35 * ambient;
			modulator = brightness + ambient;
			lightColor = vec3( 1.0, 0.5 + 0.5 * tempDirect, 0.2 + 0.8 * tempDirect );
		}

		outputColor *= lightColor * ( (modulator > 1.0) ? 1.0 : modulator );
	}
}