#version 330 core
layout(location = 0) 
out vec4 FragColor;

in vec4 fColor;

void main()
{
    if(fColor.w==0.0f) discard;
    FragColor = fColor;
}