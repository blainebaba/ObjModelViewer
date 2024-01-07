#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texCoord;

out VS_OUT {
	vec2 texCoord;
	vec3 pos;
	vec3 norm;
} vs_out;

uniform vec3 viewPos;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectMat;

void main() {
	vs_out.pos = (modelMat * vec4(pos, 1)).xyz;
	gl_Position = projectMat * viewMat * vec4(vs_out.pos, 1);
	vs_out.texCoord = texCoord;
	// norm is not changed
	vs_out.norm = norm;
}
