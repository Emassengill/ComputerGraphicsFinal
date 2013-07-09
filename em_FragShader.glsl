#version 330 core

uniform mat4 Camera = mat4(1.0f);
uniform float specexp = 0.0f;

uniform vec4 sunDirect = {0.4082f, 0.8165f, 0.4082f, 0.0f};
uniform vec3 lightPosit = {0.0f, 0.0f, 0.0f};

uniform float isInside = 0.0f;
uniform float isSun = 0.0f;

in vec3 globalPoint;
in vec3 viewerPoint;
in vec3 color;
in vec3 normal;

out vec3 outputColor;

//Utilizes Blinn-Phong shading.
void main() {

	if (isSun < 0.5f) {

		//The y component of the sunlight vector is used to
		//adjust ambient lighting to coincide
		//with night and sunset/sunrise.
		float tempDirect = pow(abs(sunDirect.y), 0.4f);
		float ambient = 0.536f * pow(sunDirect.y + 1.0f, 0.9f);

		float specular;
		float modulator;
		vec3 lightColor;

		if (isInside < 0.5f) {
			float brightness = dot(normal, sunDirect.xyz);
			if (brightness <= 0.0f) brightness = 0.0f;
			else brightness *= 0.25f * pow(sunDirect.y + 1.0f, 2.0f);
		
			ambient = 0.15f + 0.35f * ambient;
			modulator = brightness + ambient;
			if (modulator > 1.0f) modulator = 1.0f;
			//The green and blue components are attenuated as the sun reaches the horizon,
			//making the ambient lighting reder.
			lightColor = vec3( 1.0f, 0.5f + 0.5f * tempDirect, 0.2f + 0.8f * tempDirect );

			if (sunDirect.y < 0.0f) {
				specular = 0.0f;
			} else if (specexp > 0.0f) {
				vec3 halfway = normalize( sunDirect.xyz - normalize(viewerPoint) );
				specular = dot(halfway, normal);
				if (specular > 0.0f) specular = pow(specular, specexp);
				else specular = 0.0f;
			}
		} else {
			//If inside, an alternative model based on point lighting
			//is used. The ambient lighting component is determined
			//similarly to outside ambient lighting to simulate the
			//effect of lighting spilling into the house through the door.

			vec3 lightVec = lightPosit - globalPoint;
			float brightness = 54.0f * dot(normal, normalize(lightVec));
			if (brightness <= 0.0f) brightness = 0.0f;
			else brightness /= 0.1f + pow(length(lightVec), 2.0f);
		
			ambient = 0.2f + 0.3f * ambient;
			modulator = brightness + ambient;
			if (modulator > 1.0f) modulator = 1.0f;
			lightColor = vec3( 1.0f, 0.5f + 0.35f * tempDirect, 0.4f + 0.45f * tempDirect );

			if (specexp > 0.0f) {
				vec3 halfway = normalize( lightVec - normalize(viewerPoint) );
				specular = dot(halfway, normal);
				if (specular > 0.0f) specular = pow(specular, specexp);
				else specular = 0.0f;
			}
		}

		//As the view angle approaches the angle of reflection, more of the light that the viewer percieves
		//is reflected light, as opposed to scattered light, according to the specularity. Assuming a
		//specular value of 1.0 corresponds to complete, though imprecise, reflection, the specular value
		//determines the division of light between that which is specularly reflected, and that which is
		//diffusely scattered. This ensures that all color components fall in the range of [0.0, 1.0].
			outputColor = modulator * lightColor * ( specular + ( (1.0f - specular) * color ) );

	} else {

		outputColor = color;

	}

}