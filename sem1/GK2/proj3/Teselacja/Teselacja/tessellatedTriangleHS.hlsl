#define INPUT_PATCH_SIZE 16
#define OUTPUT_PATCH_SIZE 16

cbuffer cbWorld : register(b0) //Domain Shader constant buffer slot 0
{
	matrix worldMatrix;
};

struct HSInput
{
	float4 pos : POSITION;
};

struct HSPatchOutput
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct DSControlPoint
{
	float3 pos : POSITION;
};

HSPatchOutput HS_Patch(InputPatch<HSInput, INPUT_PATCH_SIZE> ip, uint patchId : SV_PrimitiveID)
{
	HSPatchOutput o;
	o.edges[0] = o.edges[1] = o.edges[2] = o.edges[3] = 8.0f;
	o.inside[0] = o.inside[1] = 8.0f;
	return o;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(OUTPUT_PATCH_SIZE)]
[patchconstantfunc("HS_Patch")]
DSControlPoint main(InputPatch<HSInput, INPUT_PATCH_SIZE> ip, uint i : SV_OutputControlPointID,
	uint patchID : SV_PrimitiveID)
{
	DSControlPoint o;
	o.pos = ip[i].pos.xyz;
	return o;
}