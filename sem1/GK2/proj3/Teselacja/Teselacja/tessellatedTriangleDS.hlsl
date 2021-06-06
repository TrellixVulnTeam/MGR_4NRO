#define OUTPUT_PATCH_SIZE 16

cbuffer cbProj : register(b0) //Domain Shader constant buffer slot 0
{
	matrix projMatrix;
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

struct PSInput
{
	float4 pos : SV_POSITION;
};

float3 DeBoor(float t, float3 B0_, float3 B1_, float3 B2_, float3 B3_)
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

	term = N1 / (A1 + B1);
	N1 = saved + A1 * term;
	saved = B1 * term;

	N2 = saved;
	saved = 0.0f;

	term = N1 / (A1 + B2);
	N1 = saved + A1 * term;
	saved = B2 * term;

	term = N2 / (A2 + B1);
	N2 = saved + A2 * term;
	saved = B1 * term;

	N3 = saved;
	saved = 0.0f;

	term = N1 / (A1 + B3);
	N1 = saved + A1 * term;
	saved = B3 * term;

	term = N2 / (A2 + B2);
	N2 = saved + A2 * term;
	saved = B2 * term;

	term = N3 / (A3 + B1);
	N3 = saved + A3 * term;
	saved = B1 * term;

	N4 = saved;

	return N1 * B0_ + N2 * B1_ + N3 * B2_ + N4 * B3_;
}

[domain("quad")]
PSInput main(HSPatchOutput factors, float2 uv : SV_DomainLocation,
	const OutputPatch<DSControlPoint, OUTPUT_PATCH_SIZE> input)
{
	PSInput o;
	float3 p1, p2, p3, p4;
	p1 = DeBoor(uv.x,input[0].pos, input[1].pos, input[2].pos, input[3].pos);
	p2 = DeBoor(uv.x,input[4].pos, input[5].pos, input[6].pos, input[7].pos);
	p3 = DeBoor(uv.x,input[8].pos, input[9].pos, input[10].pos, input[11].pos);
	p4 = DeBoor(uv.x,input[12].pos, input[13].pos, input[14].pos, input[15].pos);
	float3 viewPos = DeBoor(uv.y, p1, p2, p3, p4);
	o.pos = mul(projMatrix, float4(viewPos,1.0f));
	return o;
}