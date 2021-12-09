sampler samp;
textureCUBE envMap;

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 tex : TEXCOORD0;
};

float4 main(PSInput i) : SV_TARGET
{
    float3 color = envMap.Sample(samp , i.tex).rgb;

    return float4(pow(color,2) , 1.0f);
}
