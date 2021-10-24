#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexPos;
layout (location = 3) in vec2 aHeightTexPos;



uniform mat4 persp;
uniform mat4 view;
uniform mat4 transform;
uniform sampler2D heightTexture;
uniform float xDiff;
uniform float yDiff;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} vs_out;


void main()
{
    vec3 pos = aPos;
    if(aHeightTexPos.x>-1.0f)
    {
        pos.y = texture(heightTexture, aHeightTexPos).x;
    }
    vec3 normal = aNormal;
    if(normal.x < -1.0f)
    {
        vec2 tHeightTexPos;
         
        tHeightTexPos = vec2(aHeightTexPos.x - xDiff, aHeightTexPos.y);
        float hL = texture(heightTexture, tHeightTexPos).x;

        tHeightTexPos = vec2(aHeightTexPos.x + xDiff, aHeightTexPos.y);
        float hR = texture(heightTexture, tHeightTexPos).x;

        tHeightTexPos = vec2(aHeightTexPos.x, aHeightTexPos.y - yDiff);
        float hD = texture(heightTexture, tHeightTexPos).x;

        tHeightTexPos = vec2(aHeightTexPos.x, aHeightTexPos.y + yDiff);
        float hU = texture(heightTexture, tHeightTexPos).x;

        vec3 v1 = vec3(xDiff,hL,0.0f)-vec3(-xDiff,hR,0.0f);
        vec3 v2 = vec3(0.0f,hU,yDiff)-vec3(0.0f,hD,-yDiff);
        
        normal = normalize(cross(v1,v2));
        normal.y=-normal.y;
        //normal = vec3(0.0f,1.0f,0.0f);
    }
    vs_out.FragPos = vec3(transform * vec4(pos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(transform))) * normal;

    vec4 poss = persp * view * transform * vec4(pos, 1.0);
    poss.z = -poss.z;
    gl_Position = poss;
    vs_out.TexCoord = aTexPos;
}