cbuffer cbWorld : register(b0)
{
	matrix worldMatrix;
};

cbuffer cbView : register(b1)
{
	matrix viewMatrix;
	matrix invViewMatrix;
};

cbuffer cbProj : register(b2)
{
	matrix projMatrix;
};


struct VSInput
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct HSInput
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
};

HSInput main(VSInput i)
{
	HSInput o;
	o.pos = mul(worldMatrix, float4(i.pos, 1.0f));
	o.tex = i.tex;

	return o;
}