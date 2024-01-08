#pragma once

#include <vector>
#include <string>

#define GLT_IMPLEMENTATION
#include "gltext.h"

using namespace std;

class UI {
public:
	UI() {
		// Initialize glText
		gltInit();

		// Creating text
		addLine("W,S,A,D: move camera");
		addLine("mouse: rotate camera");
		addLine("LEFT,RIGHT: change model");
		addLine("Y: texture y axis flip");
		addLine("B: alpha blending");
		addLine("H: help info");
	}

	~UI() {
		// Deleting text
		for (GLTtext* line : lines) {
			gltDeleteText(line);
		}

		// Destroy glText
		gltTerminate();
	}

	void drawUI() {
		if (hideHelp)
			return;

		// Begin text drawing (this for instance calls glUseProgram)
		gltBeginDraw();

		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		float x = 10, y = 10;
		float scale = 1.5;
		float stride = 15;

		for (GLTtext* line : lines) {
			gltDrawText2D(line, x, y, scale);
			y += stride * scale;
		}

		// Finish drawing text
		gltEndDraw();
	}

	void keyPressCallback(GLFWwindow* window, int key, int action) {
		if (key == GLFW_KEY_H && action == GLFW_PRESS) {
			hideHelp = !hideHelp;
		}
	}

private:
	vector<GLTtext*> lines;
	bool hideHelp = false;

	void addLine(string line) {
		GLTtext* text = gltCreateText();
		gltSetText(text, line.c_str());
		lines.push_back(text);
	}
};
