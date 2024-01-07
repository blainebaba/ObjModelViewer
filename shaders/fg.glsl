#version 330 core

in VS_OUT {
	vec2 texCoord;
	vec3 pos;
} vs_out;

out vec4 FragColor;

uniform vec3 viewPos;

uniform bool use_texture_diffuse;
uniform sampler2D texture_diffuse;
uniform vec3 diffuse_color;

uniform vec3 fog_color;

void main() {
	// diffuse
	vec3 color;
	if (use_texture_diffuse) {
		color = texture(texture_diffuse, vs_out.texCoord).xyz;
	} else {
		color = diffuse_color;
	}

	// fog, clear 20, fade 20
	float depth = length(vs_out.pos);
	float fogDense = clamp((depth-20)/5, 0, 1);

	color = color * (1-fogDense) + fog_color * fogDense;
	FragColor = vec4(color,1);
}