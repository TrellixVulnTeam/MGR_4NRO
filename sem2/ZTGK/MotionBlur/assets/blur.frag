#version 420 core

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D colorTexture;
layout(binding = 1) uniform sampler2D velTexture;

const int N = 30;

void main() {
	vec2 coord = fragTexCoord;
	vec2 vel = texture(velTexture, fragTexCoord).rg/N*0.5;

	vec3 sumColor = vec3(0.0,0.0,0.0);
	int normalization = 0;
	for(int i=0; i<N; i=i+1)
	{
		int a = N-i;
		vec3 texColor = texture(colorTexture, coord).rgb;
		sumColor = sumColor + a * texColor;
		normalization = normalization +a;
		coord = coord + vel;
	}

	sumColor=sumColor/normalization;
	
	color = vec4(sumColor, 1);
}

