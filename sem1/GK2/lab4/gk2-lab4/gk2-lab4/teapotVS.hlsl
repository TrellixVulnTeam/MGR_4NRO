matrix modelMtx, modelInvTMtx, viewProjMtx;
float4 camPos;
float h0;
static const float ln = -0.693147f;

float time, xmax, vmax, thalf;

struct VSInput
{
	float3 pos : POSITION0;
	float3 norm : NORMAL0;
	float2 tex : TEXCOORD0;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION0;
	float3 norm : NORMAL0;
	float3 view : VIEWVEC0;
	float2 tex : TEXCOORD0;
};

float springHeight(float time)
{
	return xmax * exp(ln * time / thalf) * sin(vmax * time / xmax);
}

VSOutput main(VSInput i)
{
	VSOutput o;
	float4 worldPos = mul(modelMtx, float4(i.pos, 1.0f));
	float tmax = 2 * thalf;
	float _;
	worldPos.y += h0 + springHeight(modf(time / tmax, _) * tmax);
	o.view = normalize(camPos.xyz - worldPos.xyz);
	o.norm = normalize(mul(modelInvTMtx, float4(i.norm, 0.0f)).xyz);
	o.worldPos = worldPos.xyz;
	o.pos = mul(viewProjMtx, worldPos);
	o.tex = i.tex / 4.0f;
	return o;
}