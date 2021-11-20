#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec4 fColor;
uniform mat4 persp;
uniform mat4 view;
uniform mat4 transform;

void main()
{
    vec4 pos = persp * view * transform * vec4(aPos, 1.0);
    pos.z = -pos.z;
    gl_Position = pos; 

    fColor = vec4(aColor,0.5f);
}