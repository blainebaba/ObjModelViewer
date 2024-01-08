#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "shader.h"

using namespace std;
using namespace glm;

class Light {
public:
	static Light createSpotlight(vec3 pos, vec3 di, float angle) {
		Light light;

		light.type = 1;

		light.pos = pos;
		light.di = normalize(di);
		light.color = vec3(1);

		light.cutoffCos = cos(radians((angle/2)+5));
		light.cutoffStartCos = cos(radians(angle/2));

		return light;
	}

	void setupLight(Shader* shader, int index) {
		shader->setInt(uniName(index, "type"), type);
		shader->setVec3(uniName(index, "pos"), pos);
		shader->setVec3(uniName(index, "di"), di);
		shader->setVec3(uniName(index, "color"), color);
		shader->setFloat(uniName(index, "cutoffCos"), cutoffCos);
		shader->setFloat(uniName(index, "cutoffStartCos"), cutoffStartCos);
	}
private:
	vec3 pos;
	vec3 di;
	vec3 color;
	int type;

	// spot light cutoff
	float cutoffCos;
	float cutoffStartCos;

	string uniName(int index, string name) {
		return "lights[" + to_string(index) + "]." + name;
	}
};

class LightSet {
public:

	void addLight(Light light) {
		lights.push_back(light);
	}

	// setup lights in shader
	void setupLights(Shader* shader) {
		for (int i = 0; i < lights.size(); i++) {
			lights[i].setupLight(shader, i);
		}
		shader->setInt("lightNum", lights.size());
		shader->setVec3("ambient", ambient);
	}

private:
	vector<Light> lights;
	vec3 ambient = vec3(0.1);
};
