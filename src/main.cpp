#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "modelViewer.h"
#include "ui.h"

using namespace std;

ModelViewer* viewer;
UI* ui;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	viewer->mouseCallback(window, xpos, ypos);
}

void windowSizeChangeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	viewer->windowSizeChangeCallback(width, height);
}

void keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	viewer->keyPressCallback(window, key, action);
	ui->keyPressCallback(window, key, action);
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
	ui = new UI();

	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetKeyCallback(window, keyPressCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, windowSizeChangeCallback);

	vector<string> modelPaths = {
		// "D:\\code\\learn opengl\\LearnOpenGL-master\\resources\\objects\\backpack\\backpack.obj",
		"D:\\code\\learn opengl\\LearnOpenGL-master\\resources\\objects\\cyborg\\cyborg.obj",
		// "D:\\code\\learn opengl\\LearnOpenGL-master\\resources\\objects\\nanosuit\\nanosuit.obj",
		"D:\\code\\learn opengl\\LearnOpenGL-master\\resources\\objects\\planet\\planet.obj",
		"D:\\code\\learn opengl\\LearnOpenGL-master\\resources\\objects\\rock\\rock.obj",
		"D:\\code\\learn opengl\\resources\\tree2\\tree2.obj",
		"D:\\code\\learn opengl\\resources\\low_poly_tree\\low_poly_tree.obj",
		// "D:\\code\\learn opengl\\resources\\man\\man_100k.obj",
		"D:\\code\\learn opengl\\resources\\hotdog\\hotdog.obj",
		"D:\\code\\learn opengl\\resources\\car\\car.obj",
	};
	viewer->setup(modelPaths);

	while (!glfwWindowShouldClose(window)) {
		viewer->keyHoldCallback(window);

		viewer->renderLoop();

		ui->drawUI();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}