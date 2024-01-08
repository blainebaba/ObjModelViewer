#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <map>
#include <limits>
#include <math.h>

#include "shader.h"
#include "mesh.h"
#include "stb_image.h"

using namespace std;
using namespace glm;

class Model {
public:

	Model(const char* path) {
		loadModel(path);
		modelMat = calculateModelMat();
	};

	void draw(Shader* shader) {
		shader->setMat4("modelMat", modelMat);
		for (Mesh mesh : meshes) {
			mesh.draw(shader);
		}
	};

private:
	vector<Mesh> meshes;
	string directory;
	map<string, unsigned int> loadedTextures;
	mat4 modelMat;

	mat4 calculateModelMat() {
		float maxX, maxY, maxZ, minX, minY, minZ;
		maxX = maxY = maxZ = numeric_limits<float>::min();
		minX = minY = minZ = numeric_limits<float>::max();

		for (auto mesh : meshes) {
			for (auto vertex : mesh.vertices) {
				if (vertex.Position.x > maxX)
					maxX = vertex.Position.x;
				if (vertex.Position.x < minX)
					minX = vertex.Position.x;
				if (vertex.Position.y > maxY)
					maxY = vertex.Position.y;
				if (vertex.Position.y < minY)
					minY = vertex.Position.y;
				if (vertex.Position.z > maxZ)
					maxZ = vertex.Position.z;
				if (vertex.Position.z < minZ)
					minZ = vertex.Position.z;
			}
		}

		mat4 modelMat = glm::mat4(1);
		// scale it to fill 10x10x10 cube
		float xScale = 10 / (maxX - minX);
		float yScale = 10 / (maxY - minY);
		float zScale = 10 / (maxZ - minZ);
		float scale = glm::min(xScale, glm::min(yScale, zScale));
		modelMat = glm::scale(modelMat, vec3(scale, scale, scale));
		// move model to center
		modelMat = glm::translate(modelMat, vec3((minX + maxX)/2, -minY, (minZ + maxZ)/2));

		return modelMat;
	}

	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	void loadMaterials(aiMaterial* aiMtl, Material& mtl) {
		// diffuse
		loadTexture(aiMtl, aiTextureType_DIFFUSE, mtl.diffuse_texture);
		loadColor(aiMtl, mtl.diffuse_color, AI_MATKEY_COLOR_DIFFUSE);
		// specular
		loadTexture(aiMtl, aiTextureType_SPECULAR, mtl.specular_texture);
		loadColor(aiMtl, mtl.specular_color, AI_MATKEY_COLOR_SPECULAR);
		loadFloat(aiMtl, mtl.shininess, AI_MATKEY_SHININESS);
		loadFloat(aiMtl, mtl.shininess_strength, AI_MATKEY_SHININESS_STRENGTH);

		// when diffuse texture is set, diffuse color should not be black
		if (mtl.diffuse_texture != EMPTY_TEX && glm::length(vec3(mtl.diffuse_color)) < 0.01) {
			mtl.diffuse_color = vec3(1);
		}
	}

	void loadFloat(aiMaterial* mat, float& value, const char* pKey, unsigned int type, unsigned int idx) {
		float temp;
		if (AI_SUCCESS == mat->Get(pKey, type, idx, temp)) {
			value = temp;
		}
	}

	void loadColor(aiMaterial* mat, vec3& colorVec, const char* pKey, unsigned int type, unsigned int idx) {
		aiColor3D color;
		if (AI_SUCCESS == mat->Get(pKey, type, idx, color)) {
			colorVec.r = color.r;
			colorVec.g = color.g;
			colorVec.b = color.b;
		}
	}

	void loadTexture(aiMaterial* mat, aiTextureType type, unsigned int& texture) {
		if (mat->GetTextureCount(type) > 0) {
			aiString str;
			mat->GetTexture(type, 0, &str);
			string name = string(str.C_Str());
			if (loadedTextures.find(name) != loadedTextures.end()) {
				texture = loadedTextures[name];
			}
			else {
				int numChannel = 0;
				if (type == aiTextureType_DIFFUSE)
					numChannel = 4;
				else if (type == aiTextureType_SPECULAR)
					numChannel = 3;
				texture = readTextureFromFile(name.c_str(), directory, numChannel);
				loadedTextures[name] = texture;
			}
		}
	}

	unsigned int readTextureFromFile(const char* path, string directory, int expectedChannels) {
		string fileName = string(path);
		fileName = directory + "\\" + fileName;

		unsigned int texture;
		glGenTextures(1, &texture);
		cout << "loading texture " << texture << endl;
		// bind object, set target for following operation
		glBindTexture(GL_TEXTURE_2D, texture);

		int width, height, nChannel;
		// stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nChannel, expectedChannels);

		if (expectedChannels == 0)
			expectedChannels = nChannel;

		if (data) {
			GLenum format;
			if (expectedChannels == 3) {
				format = GL_RGB;
			}
			else if (expectedChannels == 4) {
				format = GL_RGBA;
			}
			else {
				std::cout << "unknown image format, number of channel: " << nChannel << std::endl;
				return -1;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
		else {
			std::cout << "fail to load image" << std::endl;
			return -1;
		}

		return texture;
	}
};

void Model::loadModel(string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "assimp loading error " << importer.GetErrorString() << endl;
	}

	directory = path.substr(0, path.find_last_of('\\'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	Material material;
	
	// vertices
	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;
		}
		vertices.push_back(vertex);
	}

	// indices
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// textures
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
		loadMaterials(aiMat, material);
	}

	return Mesh(vertices, indices, material);
}


