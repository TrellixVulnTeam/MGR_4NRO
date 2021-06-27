#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec4 fColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);

    fColor = vec4(aColor,0.5f);
}