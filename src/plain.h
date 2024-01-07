#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "global.h"

using namespace std;

class Plain {
public:
	Plain(float plainSize = 50, float lineInterval = 1, float lineWidth = 0.1) {
		this->plainSize = plainSize;
		this->lineWidth = lineWidth;
		this->lineInterval = lineInterval;
		setupPlainVAO();
		setupLineVAO();
	}
	void draw(Shader* shader) {
		glDepthFunc(GL_ALWAYS);

		shader->setMat4("modelMat", glm::mat4(1));
		Material mat;

		// plain
		glBindVertexArray(plainVAO);
		unsigned int texUnit = 0;
		mat.diffuse_color = plainColor;
		shader->setDiffuse(mat, texUnit);
		shader->setSpecular(mat, texUnit);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		// line
		glBindVertexArray(lineVAO);
		texUnit = 0;
		glDisable(GL_CULL_FACE);
		mat.diffuse_color = vec3(1);
		shader->setDiffuse(mat, texUnit);
		shader->setSpecular(mat, texUnit);
		glDrawElements(GL_TRIANGLES, 6*lineCount, GL_UNSIGNED_INT, (void*)0);
		glEnable(GL_CULL_FACE);

		glDepthFunc(GL_LESS);
	}
private:
	const vec3 plainColor = vec3(22.0f / 256, 121.0f / 256, 113.0f / 256);

	float plainSize;
	float lineWidth;
	float lineInterval;
	unsigned int plainVAO;
	unsigned int lineVAO;
	int lineCount;

	void setupPlainVAO() {
		float vertices[] = {
			// pos						// normal
			-plainSize, 0, -plainSize,  0,1,0,
			plainSize,  0, -plainSize,	0,1,0,
			-plainSize, 0, plainSize,	0,1,0,
			plainSize,  0, plainSize,	0,1,0,
		};

		unsigned int elements[] = {
			0, 2, 1,
			1, 2, 3,
		};

		glGenVertexArrays(1, &plainVAO);
		glBindVertexArray(plainVAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float)*3));

		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, &elements, GL_STATIC_DRAW);
	}

	void setupLineVAO() {
		lineCount = 0;

		vector<glm::vec3> vertices;
		vector<glm::uvec3> indices;

		float hlw = lineWidth / 2;
		for(float x = -plainSize + lineInterval; x <= plainSize - (lineInterval/2); x += lineInterval) {
			// z direction line
			vertices.push_back(glm::vec3(x-hlw, 0, -plainSize));
			vertices.push_back(glm::vec3(0, 1, 0));
			vertices.push_back(glm::vec3(x+hlw, 0, -plainSize));
			vertices.push_back(glm::vec3(0, 1, 0));
			vertices.push_back(glm::vec3(x-hlw, 0, plainSize));
			vertices.push_back(glm::vec3(0, 1, 0));
			vertices.push_back(glm::vec3(x+hlw, 0, plainSize));
			vertices.push_back(glm::vec3(0, 1, 0));

			// x direction line
			vertices.push_back(glm::vec3(-plainSize, 0, x-hlw));
			vertices.push_back(glm::vec3(0, 1, 0));
			vertices.push_back(glm::vec3(plainSize, 0, x-hlw));
			vertices.push_back(glm::vec3(0, 1, 0));
			vertices.push_back(glm::vec3(-plainSize, 0, x+hlw));
			vertices.push_back(glm::vec3(0, 1, 0));
			vertices.push_back(glm::vec3(plainSize, 0, x+hlw));
			vertices.push_back(glm::vec3(0, 1, 0));

			lineCount += 2;
		}

		int indOffset = 0;
		for (float x = -plainSize + lineInterval; x <= plainSize - (lineInterval / 2); x += lineInterval) {
			// z direction line
			indices.push_back(glm::uvec3(indOffset + 0, indOffset + 2, indOffset + 1));
			indices.push_back(glm::uvec3(indOffset + 1, indOffset + 2, indOffset + 3));
			indOffset += 4;

			// x direction line
			indices.push_back(glm::uvec3(indOffset + 0, indOffset + 2, indOffset + 1));
			indices.push_back(glm::uvec3(indOffset + 1, indOffset + 2, indOffset + 3));
			indOffset += 4;
		}


		glGenVertexArrays(1, &lineVAO);
		glBindVertexArray(lineVAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);

		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * indices.size(), glm::value_ptr(indices[0]), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float)*3));
	}
};
