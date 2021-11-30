#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;



uniform mat4 persp;
uniform mat4 view;
uniform mat4 transform;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} vs_out;


void main()
{
    vec3 pos = aPos;
    vs_out.FragPos = vec3(transform * vec4(pos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(view * transform))) * aNormal;

    vec4 poss = persp * view * transform * vec4(pos, 1.0);
    poss.z = -poss.z;
    gl_Position = poss;
    vs_out.Color = aColor;
}