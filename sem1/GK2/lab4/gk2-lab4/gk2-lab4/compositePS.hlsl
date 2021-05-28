sampler blurSampler;
Texture2D screen;
Texture2D halfscreen1;

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput i) : SV_TARGET
{
	float4 color = float4(screen.Sample(blurSampler, i.tex).rgb, 1.0f);
	float4 color2 = float4(halfscreen1.Sample(blurSampler, i.tex).rgb, 1.0f);

	return color + color2;
}