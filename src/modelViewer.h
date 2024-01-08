#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "plain.h"
#include "model.h"
#include "light.h"
#include "global.h"

class ModelViewer {
public:
	ModelViewer(Camera* camera) {
		this->camera = camera;
		this->shader = new Shader("shaders/vt.glsl", "shaders/fg.glsl");

		// create an 1x1 texture, the id should be 1 if it creates first
		createEmptyTexture();
	}

	void setup(vector<string>& modelPaths) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		lights.addLight(Light::createSpotlight(vec3(10, 2, 10), vec3(-1, 0.5, -1), 90));
		lights.addLight(Light::createSpotlight(vec3(-10, 2, 10), vec3(1, 0.5, -1), 90));

		this->plain = new Plain();

		if (modelPaths.size() == 0) {
			cout << "no model specified" << endl;
			exit(1);
		}
		for (string path : modelPaths) {
			models.push_back(Model(path.c_str()));
		}
	}

	void renderLoop() {
		glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();
		shader->setVec3("viewPos", camera->getViewPos());
		shader->setMat4("viewMat", camera->getViewMat());
		shader->setMat4("projectMat", camera->getProjectMat());
		shader->setVec3("fog_color", bgColor);
		shader->setBool("flip_y", flipY);

		lights.setupLights(shader);
		
		plain->draw(shader);

		models[curModel].draw(shader);
	}

	// continuous event during press
	void keyHoldCallback(GLFWwindow* window) {
		camera->keyboardCallBack(window);
	}

	// one event for each press
	void keyPressCallback(GLFWwindow* window, int key, int action) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
			flipY = !flipY;
		}

		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			curModel = (curModel + 1) % models.size();
		}

		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			curModel = (curModel - 1);
			if (curModel < 0)
				curModel = models.size() - 1;
		}

		if (key == GLFW_KEY_B && action == GLFW_PRESS) {
			if (blendEnabled) {
				glDisable(GL_BLEND);
				blendEnabled = false;
			}
			else {
				glEnable(GL_BLEND);
				blendEnabled = true;
			}
		}
	}

	void mouseCallback(GLFWwindow* window, float xpos, float ypos) {
		camera->mouseCallBack(window, xpos, ypos);
	}

	void windowSizeChangeCallback(int nWidth, int nHeight) {
		camera->windowSizeChanged(nWidth, nHeight);
	}

private:
	Camera* camera;
	Shader* shader;

	LightSet lights;

	Plain* plain;

	vector<Model> models;
	int curModel = 0;

	vec4 bgColor = vec4(0.1, 0.1, 0.1, 1);
	bool flipY = false;
	bool blendEnabled = true;

	void createEmptyTexture() {
		glGenTextures(1, &EMPTY_TEX);
		if (EMPTY_TEX != 1) {
			cout << "internal error, empty texture id is not 1" << endl;
		}
		glBindTexture(GL_TEXTURE_2D, EMPTY_TEX);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
};
