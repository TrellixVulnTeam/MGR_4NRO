struct PSInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION;
	float3 norm : NORMAL;
	float3 view : VIEW;
};

sampler samp : register(s0);
textureCUBE envMap : register(t0);

float4 main(PSInput i) : SV_TARGET
{
	float3 tex = normalize(i.worldPos.xyz);
	float3 col = envMap.Sample(samp, tex).rgb;
//	col = pow(col, 0.4545f);
	return float4(col,1.0f);
}