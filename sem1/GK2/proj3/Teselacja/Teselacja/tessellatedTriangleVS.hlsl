cbuffer cbWorld : register(b0) //Vertex Shader constant buffer slot 0 - matches slot in vsBilboard.hlsl
{
	matrix worldMatrix;
};

cbuffer cbView : register(b1) //Vertex Shader constant buffer slot 1 - matches slot in vsBilboard.hlsl
{
	matrix viewMatrix;
	matrix invViewMatrix;
};

cbuffer cbProj : register(b2) //Vertex Shader constant buffer slot 2 - matches slot in vsBilboard.hlsl
{
	matrix projMatrix;
};


struct VSInput
{
	float3 pos : POSITION;
};

struct HSInput
{
	float4 pos : POSITION;
};

HSInput main(VSInput i)
{
	HSInput o;
	o.pos = mul(worldMatrix, float4(i.pos, 1.0f));
	o.pos = mul(viewMatrix, o.pos);
	return o;
}