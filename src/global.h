#pragma once

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

unsigned int EMPTY_TEX = 1;

struct Material {
	unsigned int diffuse_texture = EMPTY_TEX;
	vec3 diffuse_color = vec3(1);

	unsigned int specular_texture = EMPTY_TEX;
	vec3 specular_color = vec3(1); 
	float shininess = 25;
	float shininess_strength = 1; // scale specular
};
