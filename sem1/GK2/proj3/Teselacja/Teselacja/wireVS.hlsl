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
};

struct PSInput
{
	float4 pos : SV_POSITION;
};

PSInput main(VSInput i)
{
	PSInput o;
	o.pos = mul(worldMatrix, float4(i.pos, 1.0f));
	o.pos = mul(viewMatrix, float4(o.pos.xyz, 1.0f));
	o.pos = mul(projMatrix, o.pos);
	return o;
}