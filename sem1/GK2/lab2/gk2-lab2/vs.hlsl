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

//TODO : 0.4. Change vertex shader input structure to match new vertex type
struct VSInput
{
	float3 pos : POSITION;
	float3 norm : NORMAL;
};

//TODO : 0.5. Change vertex shader output structure to store position, normal and view vectors in global coordinates
struct PSInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION;
	float3 norm : NORMAL;
	float3 view : VIEW;
};
PSInput main(VSInput i)
{
	PSInput o;
	o.worldPos = mul(worldMatrix, float4(i.pos, 1.0f)).xyz;

	//TODO : 0.6. Store global (world) position in the output

	o.pos = mul(viewMatrix, float4(o.worldPos,1.0f));
	o.pos = mul(projMatrix, o.pos);

	//TODO : 0.7. Calculate output normal and view vectors in global coordinates frame
	//Hint: you can calculate camera position from inverted view matrix

	o.norm = mul(worldMatrix, float4(i.norm, 0.0f)).xyz;
	o.norm = normalize(o.norm);

	float3 camPos = mul(invViewMatrix, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
	o.view = camPos - o.worldPos;

	return o;
}