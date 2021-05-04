#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTextureRed;
uniform sampler2D screenTextureBlue;

void main()
{ 
    vec4 red = texture(screenTextureRed, TexCoords);
    vec4 blue = texture(screenTextureBlue, TexCoords);
    if(red.x==0.0f && blue.y ==0.0f && blue.z == 0.0f)
    FragColor = vec4(1.0f,1.0f,1.0f,1.0f);
    else
    FragColor = vec4(red.x,blue.y,blue.z,1.0f);
 //FragColor = vec4(0.0f,blue.y,blue.z,1.0f);
 //FragColor = vec4(red.x,0.0f,0.0f,1.0f);
}