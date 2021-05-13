sampler blurSampler;
Texture2D screen;

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput i) : SV_TARGET
{
	return float4(screen.Sample(blurSampler, i.tex).rgb, 1.0f);
}