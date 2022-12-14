sampler blurSampler;
Texture2D halfscreen2;
float blurScale;
float2 viewportDim;

static const float blurWeights[13] = {
0.002216f,
0.008764f,
0.026995f,
0.064759f,
0.120985f,
0.176033f,
0.199471f,
0.176033f,
0.120985f,
0.064759f,
0.026995f,
0.008764f,
0.002216f
};


struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput i) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//return color;
	float2 texelSize = blurScale * 2.0f / viewportDim;
	for (int k = 0; k < 13; ++k)
		color = color + blurWeights[k] * halfscreen2.Sample(blurSampler, i.tex + float2(0.0f, ((k - 6) * 2 - 0.5f) * texelSize.y));
	return color;

}