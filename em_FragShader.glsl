#version 330 core

uniform vec4 sunDirect = {0.4082f, 0.8165f, 0.4082f, 0.0f};
uniform vec4 lightPosit = {0.0f, 0.0f, 0.0f, 1.0f};
uniform float isInside = 0.0f;
uniform float isSun = 0.0f;
uniform float occlusionTest = 0.0f;

in vec3 point;
in vec3 color;
in vec3 normal;

out vec3 outputColor;

//Utilizes diffuse Phong shading.
void main() {
	outputColor = color;

	if (isSun == 0.0f && occlusionTest == 0.0f) {
		//The y component of the sunlight vector is used
		//to adjust ambient lighting to coincide with night
		//and sunset/sunrise.
		float tempDirect = pow(abs(sunDirect.y), 0.4f);
		float ambient = 0.536f * pow(sunDirect.y + 1.0f, 0.9f);

		float modulator;
		vec3 lightColor;

		if (isInside == 1.0f) {
			vec3 lightVec = lightPosit.xyz - point;
			float brightness = 54.0f * dot(normal, normalize(lightVec));
			if (brightness <= 0.0f) brightness = 0.0f;
			else brightness /= 0.1f + pow(length(lightVec), 2.0f);
		
			ambient = 0.2f + 0.3f * ambient;
			modulator = brightness + ambient;
			lightColor = vec3( 1.0f, 0.65f + 0.35f * tempDirect, 0.4f + 0.6f * tempDirect );
		} else {
			//If inside, an alternative model based on point
			//lighting is used. The ambient lighting
			//component is determined similarly to outside
			//ambient lighting to simulate the effect
			//of light spilling into the house through the door.
			float brightness = dot(normal, sunDirect.xyz);
			if (brightness <= 0.0f) brightness = 0.0f;
			else brightness *= 0.25f * pow(sunDirect.y + 1.0f, 2.0f);
		
			ambient = 0.15 + 0.35 * ambient;
			modulator = brightness + ambient;
			lightColor = vec3( 1.0f, 0.5f + 0.5f * tempDirect, 0.2f + 0.8f * tempDirect );
		}

		outputColor *= lightColor * ( (modulator > 1.0f) ? 1.0f : modulator );
	}
}