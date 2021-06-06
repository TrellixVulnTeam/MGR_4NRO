cbuffer cbSurfaceColor : register(b0)
{
	float4 surfaceColor;
}

struct PSInput
{
	float4 pos : SV_POSITION;
};

float4 main(PSInput i) : SV_TARGET
{
	return float4(0.0f,1.0f,0.0f,1.0f);
}