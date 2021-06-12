#define INPUT_PATCH_SIZE 16
#define OUTPUT_PATCH_SIZE 16

struct Parameters
{
	int edgeTessFactor;
	int insideTessFactor;
	int useLOD;
	int displacementMapping;
	int colorTexture;
	int normalMapping;
};


cbuffer cbWorld : register(b0)
{
	matrix worldMatrix;
};

cbuffer cbView : register(b1)
{
	matrix viewMatrix;
	matrix invViewMatrix;
};

cbuffer cbParameters : register(b2)
{
	Parameters parameters;
};

struct HSInput
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct HSPatchOutput
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct DSControlPoint
{
	float3 pos : POSITION;
	float3 tex : TEXCOORD;
};

float factor_f(float z) {
	return -16.0f * log10(z * 0.1f);
}

HSPatchOutput HS_Patch(InputPatch<HSInput, INPUT_PATCH_SIZE> ip, uint patchId : SV_PrimitiveID)
{
	HSPatchOutput o;
	float factor = 1.0f;
	float factorEdgeL=1.0f;
	float factorEdgeR=1.0f;
	float factorEdgeU=1.0f;
	float factorEdgeD=1.0f;
	if (parameters.useLOD != 0)
	{
		float4 somePoint = ip[0].pos + ip[5].pos + ip[10].pos + ip[15].pos;
		somePoint /= 4.0f;
		somePoint = mul(viewMatrix, somePoint);
		factor = factor_f(somePoint.z);

		float4 dPoint = ip[5].pos;
		dPoint = mul(viewMatrix, dPoint);
		factorEdgeD = factor_f(dPoint.z);

		float4 uPoint = ip[9].pos;
		uPoint = mul(viewMatrix, uPoint);
		factorEdgeU = factor_f(uPoint.z);

		float4 lPoint = ip[5].pos;
		lPoint = mul(viewMatrix, lPoint);
		factorEdgeL = factor_f(lPoint.z);

		float4 rPoint = ip[6].pos;
		rPoint = mul(viewMatrix, rPoint);
		factorEdgeR = factor_f(rPoint.z);

	}
	
	float edgeTessFactorL = factorEdgeL * parameters.edgeTessFactor;
	float edgeTessFactorR = factorEdgeR * parameters.edgeTessFactor;
	float edgeTessFactorU = factorEdgeU * parameters.edgeTessFactor;
	float edgeTessFactorD = factorEdgeD * parameters.edgeTessFactor;
	if (edgeTessFactorL < 1.0f)edgeTessFactorL = 1.0f;
	if (edgeTessFactorR < 1.0f)edgeTessFactorR = 1.0f;
	if (edgeTessFactorU < 1.0f)edgeTessFactorU = 1.0f;
	if (edgeTessFactorD < 1.0f)edgeTessFactorD = 1.0f;

	float insideTessFactor = factor * parameters.insideTessFactor;
	if (insideTessFactor < 1.0f)insideTessFactor = 1.0f;
	
	//o.edges[0] = o.edges[1] = o.edges[2] = o.edges[3] = edgeTessFactor;
	o.edges[0] = edgeTessFactorL;
	o.edges[1] = edgeTessFactorD;
	o.edges[2] = edgeTessFactorR;
	o.edges[3] = edgeTessFactorU;
	o.inside[0] = o.inside[1] = insideTessFactor;
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
	o.tex = float3(ip[i].tex,0.0f);
	return o;
}