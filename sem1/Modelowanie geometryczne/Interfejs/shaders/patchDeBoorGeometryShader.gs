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

vec3 DeBoor(float t, vec3 B0_, vec3 B1_, vec3 B2_, vec3 B3_) 
{
	float T0 = -1.0f;
	float T1 = 0.0f;
    float T2 = 1.0f;
    float T3 = 2.0f;
    float T4 = 3.0f;
	float Tm1 = -2.0f;

	float A1 = T2 - t;
	float A2 = T3 - t;
	float A3 = T4 - t;
	float B1 = t - T1;
	float B2 = t - T0;
	float B3 = t - Tm1;

	float N1 = 1;
	float N2 = 0;
	float N3 = 0;
	float N4 = 0;

	float saved = 0.0f;
	float term = 0.0f;

	term = N1/(A1+B1);
	N1 = saved + A1*term;
	saved = B1 * term;

	N2 = saved;
	saved = 0.0f;

	term = N1/(A1+B2);
	N1 = saved + A1*term;
	saved = B2 * term;

	term = N2/(A2+B1);
	N2 = saved + A2*term;
	saved = B1 * term;

	N3 = saved;
	saved = 0.0f;

	term = N1/(A1+B3);
	N1 = saved + A1*term;
	saved = B3 * term;

	term = N2/(A2+B2);
	N2 = saved + A2*term;
	saved = B2 * term;

	term = N3/(A3+B1);
	N3 = saved + A3*term;
	saved = B1 * term;

	N4 = saved;

	return N1*B0_+N2*B1_+N3*B2_+N4*B3_;
}
  
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
    fColor=vec4(0.7f, 0.7f, 0.7f,0.5f);//gs_in[0].color;
    float from = gs_in[2].color.x;
    float to = gs_in[2].color.y;
    float splits = gs_in[2].color.z;

    float t =  gs_in[1].color.x;
    float t2 = gs_in[1].color.y;
    int vertices =4;

	float u_start = gs_in[3].color.x;
    float v_start = gs_in[3].color.y;
    float u_size = gs_in[3].color.z;
    float v_size = gs_in[1].color.z;

    vec3 point1_0;
    vec3 point1_1;
    vec3 point1_2;
    vec3 point1_3;
    vec3 point2_0;
    vec3 point2_1;
    vec3 point2_2;
    vec3 point2_3;

    point1_0 = vec3(gl_in[0].gl_Position);
    point1_1 = vec3(gl_in[1].gl_Position);
    point1_2 = vec3(gl_in[2].gl_Position);
    point1_3 = vec3(gl_in[3].gl_Position);


   
   point1_0 = DeBoor(t ,vec3(gl_in[0].gl_Position),vec3(gs_in[0].pos2),vec3(gs_in[0].pos3),vec3(gs_in[0].pos4));
   point2_0 = DeBoor(t2,vec3(gl_in[0].gl_Position),vec3(gs_in[0].pos2),vec3(gs_in[0].pos3),vec3(gs_in[0].pos4));
   
   point1_1 = DeBoor(t ,vec3(gl_in[1].gl_Position),vec3(gs_in[1].pos2),vec3(gs_in[1].pos3),vec3(gs_in[1].pos4));
   point2_1 = DeBoor(t2,vec3(gl_in[1].gl_Position),vec3(gs_in[1].pos2),vec3(gs_in[1].pos3),vec3(gs_in[1].pos4));
   
   point1_2 = DeBoor(t ,vec3(gl_in[2].gl_Position),vec3(gs_in[2].pos2),vec3(gs_in[2].pos3),vec3(gs_in[2].pos4));
   point2_2 = DeBoor(t2,vec3(gl_in[2].gl_Position),vec3(gs_in[2].pos2),vec3(gs_in[2].pos3),vec3(gs_in[2].pos4));
   
   point1_3 = DeBoor(t ,vec3(gl_in[3].gl_Position),vec3(gs_in[3].pos2),vec3(gs_in[3].pos3),vec3(gs_in[3].pos4));
   point2_3 = DeBoor(t2,vec3(gl_in[3].gl_Position),vec3(gs_in[3].pos2),vec3(gs_in[3].pos3),vec3(gs_in[3].pos4));
    
	float v1 = v_start + t*v_size;
    float v2 = v_start + t2*v_size;
	float u;
    vec4 tex;
    
    float diff = (to-from)/splits;
    t=from;
    for(int i=0; i<=splits;++i)
    {
        u = u_start + t * u_size;
        gl_Position = vec4(DeBoor(t,point1_0,point1_1,point1_2,point1_3),1.0f);
        gl_Position = persp*view*gl_Position;
		tex = texture(trimTexture, vec2(u, v1));
        fColor.w=0.5f;
        if(tex.x<0.5f)
            fColor.w=0.0f;
        EmitVertex();

        gl_Position = vec4(DeBoor(t,point2_0,point2_1,point2_2,point2_3),1.0f);
        gl_Position = persp*view*gl_Position;
		tex = texture(trimTexture, vec2(u, v2));
        fColor.w=0.5f;
        if(tex.x<0.5f)
            fColor.w=0.0f;
        EmitVertex();
        t+=diff;
    }
    EndPrimitive();

}    