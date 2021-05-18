struct PSInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float3 view : VIEW;
};

sampler samp : register(s0);
texture2D duckTex : register(t0);

float4 main(PSInput i) : SV_TARGET
{
	float3 col = duckTex.Sample(samp, i.tex).rgb;
	return float4(col,1.0f);
}