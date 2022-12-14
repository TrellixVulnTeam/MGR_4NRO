#version 330 core
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 256) out;

uniform mat4 persp;
uniform mat4 view;
uniform sampler2D trimTexture;


in VS_OUT {
    vec3 color;
    vec3 pos2;
    vec3 pos3;
    vec3 pos4;
} gs_in[4];

out vec4 fColor;
  
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
    fColor=vec4(gs_in[0].color,0.5f);
    float coeffs_x[4];
    float coeffs_y[4];
    float coeffs_z[4];
    float coeffs2_x[4];
    float coeffs2_y[4];
    float coeffs2_z[4];
    float from = gs_in[2].color.x;
    float to = gs_in[2].color.y;
    float splits = gs_in[2].color.z;

    float t =  gs_in[1].color.x;
    float t2 = gs_in[1].color.y;

    float u_start = gs_in[3].color.x;
    float v_start = gs_in[3].color.y;
    float u_size = gs_in[3].color.z;
    float v_size = gs_in[1].color.z;

    int vertices =4;

    vec3 point1_0;
    vec3 point1_1;
    vec3 point1_2;
    vec3 point1_3;
    vec3 point2_0;
    vec3 point2_1;
    vec3 point2_2;
    vec3 point2_3;

   coeffs_x[0]=gl_in[0].gl_Position.x;
   coeffs_y[0]=gl_in[0].gl_Position.y;
   coeffs_z[0]=gl_in[0].gl_Position.z;
   coeffs_x[1]=gs_in[0].pos2.x;
   coeffs_y[1]=gs_in[0].pos2.y;
   coeffs_z[1]=gs_in[0].pos2.z;
   coeffs_x[2]=gs_in[0].pos3.x;
   coeffs_y[2]=gs_in[0].pos3.y;
   coeffs_z[2]=gs_in[0].pos3.z;
   coeffs_x[3]=gs_in[0].pos4.x;
   coeffs_y[3]=gs_in[0].pos4.y;
   coeffs_z[3]=gs_in[0].pos4.z;
   point1_0.x = DeCasteljau(coeffs_x,t,vertices);
   point1_0.y = DeCasteljau(coeffs_y,t,vertices);
   point1_0.z = DeCasteljau(coeffs_z,t,vertices);
   point2_0.x = DeCasteljau(coeffs_x,t2,vertices);
   point2_0.y = DeCasteljau(coeffs_y,t2,vertices);
   point2_0.z = DeCasteljau(coeffs_z,t2,vertices);
    
   coeffs_x[0]=gl_in[1].gl_Position.x;
   coeffs_y[0]=gl_in[1].gl_Position.y;
   coeffs_z[0]=gl_in[1].gl_Position.z;
   coeffs_x[1]=gs_in[1].pos2.x;
   coeffs_y[1]=gs_in[1].pos2.y;
   coeffs_z[1]=gs_in[1].pos2.z;
   coeffs_x[2]=gs_in[1].pos3.x;
   coeffs_y[2]=gs_in[1].pos3.y;
   coeffs_z[2]=gs_in[1].pos3.z;
   coeffs_x[3]=gs_in[1].pos4.x;
   coeffs_y[3]=gs_in[1].pos4.y;
   coeffs_z[3]=gs_in[1].pos4.z;
   point1_1.x = DeCasteljau(coeffs_x,t,vertices);
   point1_1.y = DeCasteljau(coeffs_y,t,vertices);
   point1_1.z = DeCasteljau(coeffs_z,t,vertices);
   point2_1.x = DeCasteljau(coeffs_x,t2,vertices);
   point2_1.y = DeCasteljau(coeffs_y,t2,vertices);
   point2_1.z = DeCasteljau(coeffs_z,t2,vertices);
       
   coeffs_x[0]=gl_in[2].gl_Position.x;
   coeffs_y[0]=gl_in[2].gl_Position.y;
   coeffs_z[0]=gl_in[2].gl_Position.z;
   coeffs_x[1]=gs_in[2].pos2.x;
   coeffs_y[1]=gs_in[2].pos2.y;
   coeffs_z[1]=gs_in[2].pos2.z;
   coeffs_x[2]=gs_in[2].pos3.x;
   coeffs_y[2]=gs_in[2].pos3.y;
   coeffs_z[2]=gs_in[2].pos3.z;
   coeffs_x[3]=gs_in[2].pos4.x;
   coeffs_y[3]=gs_in[2].pos4.y;
   coeffs_z[3]=gs_in[2].pos4.z;
   point1_2.x = DeCasteljau(coeffs_x,t,vertices);
   point1_2.y = DeCasteljau(coeffs_y,t,vertices);
   point1_2.z = DeCasteljau(coeffs_z,t,vertices);
   point2_2.x = DeCasteljau(coeffs_x,t2,vertices);
   point2_2.y = DeCasteljau(coeffs_y,t2,vertices);
   point2_2.z = DeCasteljau(coeffs_z,t2,vertices);
        
   coeffs_x[0]=gl_in[3].gl_Position.x;
   coeffs_y[0]=gl_in[3].gl_Position.y;
   coeffs_z[0]=gl_in[3].gl_Position.z;
   coeffs_x[1]=gs_in[3].pos2.x;
   coeffs_y[1]=gs_in[3].pos2.y;
   coeffs_z[1]=gs_in[3].pos2.z;
   coeffs_x[2]=gs_in[3].pos3.x;
   coeffs_y[2]=gs_in[3].pos3.y;
   coeffs_z[2]=gs_in[3].pos3.z;
   coeffs_x[3]=gs_in[3].pos4.x;
   coeffs_y[3]=gs_in[3].pos4.y;
   coeffs_z[3]=gs_in[3].pos4.z;
   point1_3.x = DeCasteljau(coeffs_x,t,vertices);
   point1_3.y = DeCasteljau(coeffs_y,t,vertices);
   point1_3.z = DeCasteljau(coeffs_z,t,vertices);
   point2_3.x = DeCasteljau(coeffs_x,t2,vertices);
   point2_3.y = DeCasteljau(coeffs_y,t2,vertices);
   point2_3.z = DeCasteljau(coeffs_z,t2,vertices);



    coeffs_x[0]=point1_0.x;
    coeffs_y[0]=point1_0.y;
    coeffs_z[0]=point1_0.z;
    coeffs_x[1]=point1_1.x;
    coeffs_y[1]=point1_1.y;
    coeffs_z[1]=point1_1.z;
    coeffs_x[2]=point1_2.x;
    coeffs_y[2]=point1_2.y;
    coeffs_z[2]=point1_2.z;
    coeffs_x[3]=point1_3.x;
    coeffs_y[3]=point1_3.y;
    coeffs_z[3]=point1_3.z;

    coeffs2_x[0]=point2_0.x;
    coeffs2_y[0]=point2_0.y;
    coeffs2_z[0]=point2_0.z;
    coeffs2_x[1]=point2_1.x;
    coeffs2_y[1]=point2_1.y;
    coeffs2_z[1]=point2_1.z;
    coeffs2_x[2]=point2_2.x;
    coeffs2_y[2]=point2_2.y;
    coeffs2_z[2]=point2_2.z;
    coeffs2_x[3]=point2_3.x;
    coeffs2_y[3]=point2_3.y;
    coeffs2_z[3]=point2_3.z;

    float v1 = v_start + t*v_size;
    float v2 = v_start + t2*v_size;
    float diff = (to-from)/splits;
    t=from;
    float u;
    vec4 tex;

    for(int i=0; i<=splits;++i)
    {
        u = u_start + t * u_size;
        gl_Position.x = DeCasteljau(coeffs_x,t,vertices);
        gl_Position.y = DeCasteljau(coeffs_y,t,vertices);
        gl_Position.z = DeCasteljau(coeffs_z,t,vertices);
        gl_Position.w=1.0;
        gl_Position = persp*view*gl_Position;
        tex = texture(trimTexture, vec2(u, v1));
        fColor.w=0.5f;
        if(tex.x<0.5f)
            fColor.w=0.0f;
        EmitVertex();

        gl_Position.x = DeCasteljau(coeffs2_x,t,vertices);
        gl_Position.y = DeCasteljau(coeffs2_y,t,vertices);
        gl_Position.z = DeCasteljau(coeffs2_z,t,vertices);
        gl_Position.w=1.0;
        gl_Position = persp*view*gl_Position;
        tex = texture(trimTexture, vec2(u, v2));
        fColor.w = 0.5f;
        if(tex.x<0.5f)
            fColor.w=0.0f;
        EmitVertex();
        t+=diff;
    }
    EndPrimitive();

}    