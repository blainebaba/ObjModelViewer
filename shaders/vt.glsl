#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texCoord;

out VS_OUT {
	vec2 texCoord;
	vec3 pos;
} vs_out;

uniform vec3 viewPos;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectMat;

void main() {
	gl_Position = projectMat * viewMat * modelMat * vec4(pos, 1);
	vs_out.texCoord = texCoord;
	vs_out.pos = pos;
}
