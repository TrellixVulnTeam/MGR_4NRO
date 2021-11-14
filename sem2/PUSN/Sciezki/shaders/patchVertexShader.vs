#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 pos2;
layout (location = 3) in vec3 pos3;
layout (location = 4) in vec3 pos4;

out VS_OUT {
    vec3 color;
    vec3 pos2;
    vec3 pos3;
    vec3 pos4;
} vs_out;


void main()
{
    gl_Position = vec4(aPos, 1.0);
    vs_out.color = aColor;
    vs_out.pos2 = pos2;
    vs_out.pos3 = pos3;
    vs_out.pos4 = pos4;
}