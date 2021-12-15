#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec4 fColor;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 persp;

void main()
{
    vec4 pos = persp * view * vec4(aPos,1.0);
    gl_Position = pos;

    fColor = vec4(aColor,0.5f);
}