#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "shader.h"
#include "global.h"

using namespace glm;
using namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material mat;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) {
		
		this->vertices = vertices;
		this->indices = indices;
		this->mat = material;

		setupMesh();
	}

	void draw(Shader* shader) {
		unsigned int textureUnit = 0;

		shader->setDiffuse(mat, textureUnit);
		shader->setSpecular(mat, textureUnit);

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void setInstances(glm::vec3 posArray[], int count) {
		glBindVertexArray(VAO);
		if (instanceVBO == 0) {
			glGenBuffers(1, &instanceVBO);
		}
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count, &posArray[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glVertexAttribDivisor(3, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		instancesNum = count;
	}

private:
	unsigned int VAO;
	unsigned int instanceVBO = 0;
	int instancesNum = 0;

	void setupMesh();
};




void Mesh::setupMesh() {
	// create VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO: vertex buffer objects, stores vertex values in a buffer
	unsigned int VBO;
	glGenBuffers(1, &VBO); // first arg: number of buffers to generate
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// EBO: element buffer object, stores vertex indexes
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// link vertex attributes
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,Normal));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
	glEnableVertexAttribArray(2);

	// end of this VAO
	glBindVertexArray(0);
}

