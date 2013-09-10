#version 330 core

uniform mat4 Camera = mat4(1.0f);
uniform float specexp = 0.0f;

uniform vec3 directLight = {0.4082f, 0.8165f, 0.4082f};
uniform vec3 pointLight = {0.0f, 0.0f, 0.0f};

uniform uint isInside = 0u;
uniform uint isSun = 0u;
uniform uint shadowMap = 0u;

in Interface {
	vec3 global;
	vec3 viewer;
	vec3 shadow;
	vec3 color;
	vec3 normal;
} fragment;

out vec3 outputColor;

//Utilizes Blinn-Phong shading.
void main() {

	if ( ! bool(shadowMap) && ! bool(isSun) ) { //if not rendering the sun, and not shadow mapping

		//The y component of the sunlight vector is used to
		//adjust ambient lighting to coincide
		//with night and sunset/sunrise.
		float tempDirect = pow(abs(directLight.y), 0.4f);
		float ambient = 0.536f * pow(directLight.y + 1.0f, 0.9f);

		float specular;
		float modulator;
		vec3 lightColor;

		if ( ! bool(isInside) ) { //if outside

			float brightness = dot(fragment.normal, directLight);
			if (brightness <= 0.0f) brightness = 0.0f;
			else brightness *= 0.25f * pow(directLight.y + 1.0f, 2.0f);
		
			ambient = 0.15f + 0.35f * ambient;
			modulator = brightness + ambient;
			if (modulator > 1.0f) modulator = 1.0f;
			//The green and blue components are attenuated as the sun reaches the horizon,
			//making the ambient lighting redder.
			lightColor = vec3( 1.0f, 0.5f + 0.5f * tempDirect, 0.2f + 0.8f * tempDirect );

			if (directLight.y < 0.0f) { //if sun is on opposite side of polygon, no specular lighting
				specular = 0.0f;
			} else if (specexp > 0.0f) { //if specexp is 0.0, then the material is not reflective
				vec3 halfway = normalize( directLight - normalize(fragment.viewer) );
				specular = dot(halfway, fragment.normal);
				if (specular > 0.0f) specular = pow(specular, specexp);
				else specular = 0.0f;
			}

		} else { //if inside

			//If inside, an alternative model based on point lighting
			//is used. The ambient lighting component is determined
			//similarly to outside ambient lighting to simulate the
			//effect of lighting spilling into the house through the door.

			vec3 lightVec = pointLight - fragment.global;
			float brightness = 54.0f * dot(fragment.normal, normalize(lightVec));
			if (brightness <= 0.0f) brightness = 0.0f;
			else brightness /= 0.01f + pow(length(lightVec), 2.0f);
		
			ambient = 0.2f + 0.3f * ambient;
			modulator = brightness + ambient;
			if (modulator > 1.0f) modulator = 1.0f;
			lightColor = vec3( 1.0f, 0.5f + 0.35f * tempDirect, 0.4f + 0.45f * tempDirect );

			if (specexp > 0.0f) {
				vec3 halfway = normalize( lightVec - normalize(fragment.viewer) );
				specular = dot(halfway, fragment.normal);
				if (specular > 0.0f) specular = pow(specular, specexp);
				else specular = 0.0f;
			}

		}

		//As the view angle approaches the angle of reflection, more of the light that the viewer percieves
		//is reflected light, as opposed to scattered light, according to the specularity. Assuming a
		//specular value of 1.0 corresponds to complete, though imprecise, reflection, the specular value
		//determines the division of light between that which is specularly reflected, and that which is
		//diffusely scattered. This ensures that all color components fall in the range of [0.0, 1.0].
			outputColor = modulator * lightColor * ( specular + ( (1.0f - specular) * fragment.color ) );

	} else { //if rendering the sun, or shadow mapping

		outputColor = fragment.color;

	}

}