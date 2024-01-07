#version 330 core

in VS_OUT {
	vec2 texCoord;
	vec3 pos;
	vec3 norm;
} vs_out;

struct Light {
	int type; // 1: spot light

	vec3 pos;	
	vec3 di;
	vec3 color;

	float cutoffCos;
	float cutoffStartCos;
};
// at most 10 lights
uniform Light lights[10];
uniform vec3 ambient;
uniform int lightNum;
uniform vec3 viewPos;

out vec4 FragColor;

struct Material {
	sampler2D tex_diffuse_texture;
	vec3 tex_diffuse_color;

	sampler2D tex_specular_texture;
	vec3 tex_specular_color;
	float tex_spec_shininess;
	float tex_spec_scale;
};

uniform Material mtl;

uniform vec3 fog_color;
uniform bool flip_y;

vec3 readTexture(sampler2D sampler, vec2 texCoord, vec3 texColor) {
	vec3 color;
	if (textureSize(sampler, 0).x > 1) {
		color = texture(sampler, texCoord).xyz;
	} else {
		color = vec3(1);
	}
	return color * texColor;
}

void main() {
	vec2 texCoord = vs_out.texCoord;
	if (flip_y)
		texCoord.y = 1 - texCoord.y;

	vec3 color = vec3(0);

	vec3 viewVec = normalize(viewPos - vs_out.pos);
	vec3 norm = normalize(vs_out.norm);

	// lights
	vec3 diffuseLight = ambient;
	vec3 specularLight = vec3(0);

	for (int i = 0; i<lightNum; i++) {
		if (lights[i].type == 1) {
			// spot light
			vec3 lightVec = normalize(lights[i].pos - vs_out.pos);
			float cutoffFactor = (dot(-lightVec, lights[i].di) - lights[i].cutoffCos) / (lights[i].cutoffStartCos - lights[i].cutoffCos);
			cutoffFactor = clamp(cutoffFactor, 0, 1);

			if (cutoffFactor != 0) {
				vec3 reflectVec = reflect(-lightVec, norm);

				float diffLight= clamp(dot(lightVec, norm),0,1);
				float specLight;
				if (mtl.tex_spec_shininess > 0) {
					specLight = pow(clamp(dot(reflectVec, viewVec),0,1), mtl.tex_spec_shininess);
					specLight *= mtl.tex_spec_scale;
				} else {
					specLight = 0;
				}

				diffLight = cutoffFactor * diffLight;
				specLight = cutoffFactor * specLight;

				diffuseLight += lights[i].color * diffLight;
				specularLight += lights[i].color * specLight;
			}
		}
	}

	// diffuse
	vec3 diffuseColor = readTexture(mtl.tex_diffuse_texture, texCoord, mtl.tex_diffuse_color);
	color += diffuseColor * diffuseLight;

	// specular
	vec3 specColor = readTexture(mtl.tex_specular_texture, texCoord, mtl.tex_specular_color);
	color += specColor * specularLight;

	// fog, clear 20, fade 20
	float depth = length(vs_out.pos);
	float fogDense = clamp((depth-20)/5, 0, 1);
	color = color * (1-fogDense) + fog_color * fogDense;

	FragColor = vec4(color,1);
	// FragColor = vec4(diffuseLight, 1);
}