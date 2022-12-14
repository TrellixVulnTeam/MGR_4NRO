#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_separate_shader_objects : require

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(location = 10) uniform mat4 model;
layout(location = 11) uniform mat4 view;
layout(location = 12) uniform mat4 perspective;
layout(location = 13) uniform mat4 oldModel;
layout(location = 14) uniform mat4 oldView;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec4 newPos;
layout(location = 4) out vec4 oldPos;

void main() {
	newPos =  perspective * view * model * vec4(position, 1.0);
	oldPos =  perspective * oldView * oldModel * vec4(position, 1.0);

	gl_Position = perspective * view * model * vec4(position, 1.0);
	fragPos = vec3(view * model * vec4(position, 1));
	fragNormal = vec3(view * model* vec4(normal, 0));
	fragTexCoord = texCoord;
}
