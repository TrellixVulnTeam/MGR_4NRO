cbuffer cbWorld : register(b0) //Vertex Shader constant buffer slot 0
{
	matrix worldMatrix;
};

cbuffer cbView : register(b1) //Vertex Shader constant buffer slot 1
{
	matrix viewMatrix;
	matrix invViewMatrix;
};

cbuffer cbProj : register(b2) //Vertex Shader constant buffer slot 2
{
	matrix projMatrix;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PSInput main( float3 pos : POSITION )
{
	PSInput o;
	o.pos = float4(pos, 1.0f);
	o.tex = pos.xy;

	matrix ModelView = mul(viewMatrix, worldMatrix);
	// Column 0:
	ModelView[0][0] = 1;
	ModelView[0][1] = 0;
	ModelView[0][2] = 0;

	//// Column 1:
	ModelView[1][0] = 0;
	ModelView[1][1] = 1;
	ModelView[1][2] = 0;

	// Column 2:
	ModelView[2][0] = 0;
	ModelView[2][1] = 0;
	ModelView[2][2] = 1;
	o.pos = mul(ModelView, o.pos);
	o.pos = mul(projMatrix, o.pos);

	//TODO : 1.31. Calculate on-screen position of billboard vertex

	return o;
}