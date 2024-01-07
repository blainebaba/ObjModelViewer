#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "modelViewer.h"

using namespace std;

ModelViewer* viewer;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	viewer->mouseCallback(window, xpos, ypos);
}

void windowSizeChangeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	viewer->windowSizeChangeCallback(width, height);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = 1800, height = 1200;
	GLFWwindow* window = glfwCreateWindow(width, height, "Obj Model Viewer", NULL, NULL);
	if (window == NULL) {
		cout << "Window creation failed." << endl;
		exit(1);
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(1);
	}

	glViewport(0, 0, width, height);

	Camera camera = Camera(glm::radians(45.0f), width, height);
	viewer = new ModelViewer(&camera);

	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, windowSizeChangeCallback);

	viewer->setup();

	while (!glfwWindowShouldClose(window)) {
		viewer->keyboardCallback(window);

		viewer->renderLoop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}