#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "global.h"

using namespace std;

// wrapper of a shader program
class Shader {
public:
	unsigned int ID;

	// init shader from file
	Shader(const char* vertexPath, const char* fragmentPath);
	// use this shader
	void use();
	
	// set uniform attributes
	void setBool(const std::string &name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);

	void setMat4(const std::string& name, const glm::mat4& mat) {
		int location = glGetUniformLocation(ID, name.c_str());
		glUseProgram(ID);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void setVec3(const std::string& name, glm::vec3 vec) {
		setVec3(name, glm::value_ptr(vec));
	}

	void setVec3(const std::string& name, float values[3]) {
		int location = glGetUniformLocation(ID, name.c_str());
		glUseProgram(ID);
		glUniform3fv(location, 1, values);
	}

	void setDiffuse(Material& mat, unsigned int& texUnit) {
		glActiveTexture(GL_TEXTURE0 + texUnit);
		glBindTexture(GL_TEXTURE_2D, mat.diffuse_texture);
		setInt(DIFF_TEX, texUnit);
		texUnit++;

		setVec3(DIFF_COLO, mat.diffuse_color);
	}

	void setSpecular(Material& mat, unsigned int& texUnit) {
		glActiveTexture(GL_TEXTURE0 + texUnit);
		glBindTexture(GL_TEXTURE_2D, mat.specular_texture);
		setInt(SPEC_TEX, texUnit);
		texUnit++;

		setVec3(SPEC_COLO, mat.specular_color);
		setFloat(SPEC_SHINE, mat.shininess);
		setFloat(SPEC_SHINE_S, mat.shininess_strength);
	}

private:
	const string DIFF_TEX = "mtl.tex_diffuse_texture";
	const string DIFF_COLO = "mtl.tex_diffuse_color";

	const string SPEC_TEX = "mtl.tex_specular_texture";
	const string SPEC_COLO = "mtl.tex_specular_color";
	const string SPEC_SHINE = "mtl.tex_spec_shininess";
	const string SPEC_SHINE_S = "mtl.tex_spec_scale";

};

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	// 1. read source code from files
	std::string vShaderCodeString;
	std::string  fShaderCodeString;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vShaderCodeString = vShaderStream.str();
		fShaderCodeString = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "read shader souce code fail" << std::endl;
	}
	const char* vShaderCode = vShaderCodeString.c_str();
	const char* fShaderCode = fShaderCodeString.c_str();

	// 2. compile and link shader
	int success;
	char infoLog[512];

	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "shader compiling failed\n" << infoLog << std::endl;
	}

	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "shader compiling failed\n" << infoLog << std::endl;
		exit(1);
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "shader program linking failed\n" << infoLog << std::endl;
		exit(1);
	}

	// delete shaders after link
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUseProgram(ID);
	glUniform1i(location, (int)value);
}

void Shader::setInt(const std::string& name, int value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUseProgram(ID);
	glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUseProgram(ID);
	glUniform1f(location, value);
}
