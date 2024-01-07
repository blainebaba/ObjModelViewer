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

class ModelViewer {
public:
	
	ModelViewer(Camera* camera) {
		this->camera = camera;
		this->shader = new Shader("shaders/vt.glsl", "shaders/fg.glsl");

		this->plain = new Plain();
		// this->model = new Model("D:\\code\\learn opengl\\LearnOpenGL-master\\resources\\objects\\backpack\\backpack.obj");
		this->model = new Model("D:\\code\\learn opengl\\LearnOpenGL-master\\resources\\objects\\backpack\\backpack.obj");
	}

	void setup() {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glEnable(GL_MULTISAMPLE);
	}

	void renderLoop() {
		glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();
		shader->setVec3("viewPos", camera->getViewPos());
		shader->setMat4("viewMat", camera->getViewMat());
		shader->setMat4("projectMat", camera->getProjectMat());
		shader->setVec3("fog_color", bgColor);
		
		plain->draw(shader);

		model->draw(shader);
	}

	void keyboardCallback(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		camera->keyboardCallBack(window);
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

	Plain* plain;

	Model* model;

	vec4 bgColor = vec4(0.2, 0.2, 0.2, 1);
};
