#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} gs_in[];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main() {    
    FragPos = gs_in[0].FragPos;
    Normal = gs_in[0].Normal;
    TexCoord = gs_in[0].TexCoord;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    FragPos = gs_in[1].FragPos;
    Normal = gs_in[1].Normal;
    TexCoord = gs_in[1].TexCoord;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    FragPos = gs_in[2].FragPos;
    Normal = gs_in[2].Normal;
    TexCoord = gs_in[2].TexCoord;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
}