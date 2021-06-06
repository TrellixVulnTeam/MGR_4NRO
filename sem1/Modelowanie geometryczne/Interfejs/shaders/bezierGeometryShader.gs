#version 330 core
layout (lines_adjacency) in;
layout (line_strip, max_vertices = 256) out;

uniform mat4 persp;
uniform mat4 view;


in VS_OUT {
    vec3 color;
} gs_in[4];

out vec3 fColor;
  
float DeCasteljau(float coeffs_t[4], float t, int vertices)
{
	for (int i = vertices; i > 0; --i)
	{
		for (int j = 0; j < i - 1; ++j)
		{
			coeffs_t[j] = (1 - t) * coeffs_t[j] + t * coeffs_t[j + 1];
		}
	}
	return coeffs_t[0];
}

void main() {    
    fColor=vec3(1.0f,1.0f,1.0f);//gs_in[0].color;
    float coeffs_x[4];
    float coeffs_y[4];
    float coeffs_z[4];
    float from = gs_in[1].color.y;
    float to = gs_in[1].color.z;

    coeffs_x[0]=gl_in[0].gl_Position.x;
    coeffs_y[0]=gl_in[0].gl_Position.y;
    coeffs_z[0]=gl_in[0].gl_Position.z;

    coeffs_x[1]=gl_in[1].gl_Position.x;
    coeffs_y[1]=gl_in[1].gl_Position.y;
    coeffs_z[1]=gl_in[1].gl_Position.z;

    coeffs_x[2]=gl_in[2].gl_Position.x;
    coeffs_y[2]=gl_in[2].gl_Position.y;
    coeffs_z[2]=gl_in[2].gl_Position.z;

    coeffs_x[3]=gl_in[3].gl_Position.x;
    coeffs_y[3]=gl_in[3].gl_Position.y;
    coeffs_z[3]=gl_in[3].gl_Position.z;

    if(gs_in[1].color.x==-1.0)
    {
        EndPrimitive();
        return;
    }

    int vertices =4;
    if(gs_in[3].color.x==-1.0) vertices =3;
    if(gs_in[2].color.x==-1.0) vertices =2;
    float diff = (to-from)/250.0;
    float t=from;
    for(int i=0; i<=250;++i)
    {
        gl_Position.x = DeCasteljau(coeffs_x,t,vertices);
        gl_Position.y = DeCasteljau(coeffs_y,t,vertices);
        gl_Position.z = DeCasteljau(coeffs_z,t,vertices);
        gl_Position.w=1.0;
        gl_Position = persp*view*gl_Position;
        EmitVertex();
        t+=diff;
    }
    EndPrimitive();

    
}    