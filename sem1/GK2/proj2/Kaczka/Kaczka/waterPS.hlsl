cbuffer cbSurfaceColor : register(b0)
{
	float4 surfaceColor;
};


Texture2D colorMap : register(t0);
textureCUBE envMap  : register(t1);
SamplerState colorSampler : register(s0);

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex: NORMAL;
	float3 worldPos : POSITION0;
	float3 viewVec : TEXCOORD0;
};


float3 intersectRay(float3 p, float3 d)
{
    float tx = max((1 - p.x) / d.x, (-1 - p.x) / d.x);
    float ty = max((1 - p.y) / d.y, (-1 - p.y) / d.y);
    float tz = max((1 - p.z) / d.z, (-1 - p.z) / d.z);

    float t = min(tx, min(ty, tz));
    return p + d * t;
}

float fresnel(float n1, float n2, float3 N, float3 V)
{
    float f0 = (n2 - n1) / (n1 + n2);
    f0 *= f0;
    if (dot(N, V) < 0) N = -N;
    float costh = max(dot(N, V), 0);
    return f0 + (1.0f - f0) * pow((1.0f - costh), 5);
}



float4 main(PSInput i) : SV_TARGET
{
    float3 viewVec = normalize(i.viewVec);
    float3 norm = normalize(colorMap.SampleLevel(colorSampler, i.tex, 0).rgb * 2.0f - 1.0f);
	float3 pos = i.worldPos;
	pos /= 4.0f;
    float n1 = 1.0f;
    float n2 = 4.0f / 3.0f;

    float3 refl = reflect(-viewVec, norm);
    float refrCoeff = n1 / n2;
    if (dot(norm, viewVec) < 0) { refrCoeff = n2 / n1; norm = -norm; }
    float3 refr = refract(-viewVec, norm, refrCoeff);

    float fres = fresnel(n1, n2, norm, viewVec);
    //fres = 1.0f - fres;
    float3 color;


    float3 tRefl = intersectRay(pos, refl);
    float3 colorRefl = envMap.Sample(colorSampler, tRefl).rgb;
    float3 tRefr = intersectRay(pos, refr);
    float3 colorRefr = envMap.Sample(colorSampler, tRefr).rgb;
    if (any(refr))
    {
        color = fres * colorRefl + (1.0f - fres) * colorRefr;
        if (dot(norm, viewVec) < 0)
        {
            tRefr.y = -tRefr.y;
            color = envMap.Sample(colorSampler , tRefr).rgb;
        }
    }
    else
    {
        color = colorRefl;
    }

   // color = pow(color, 0.4545f);
    return float4(color, 1.0f);
}