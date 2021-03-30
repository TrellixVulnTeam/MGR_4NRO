cbuffer cbSurfaceColor : register(b0) //Pixel Shader constant buffer slot 0
{
	float4 surfaceColor;
}

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput i) : SV_TARGET
{
	//TODO : 1.32. Calculate billboard pixel color

	float a = 1.0f;
	float d = sqrt(i.tex.x * i.tex.x + i.tex.y * i.tex.y);
	float c = 1 - d / (0.5f * a);
	if (c < 0.0f) c = 0.0f;

	return float4(c*surfaceColor); //Replace with correct implementation
}