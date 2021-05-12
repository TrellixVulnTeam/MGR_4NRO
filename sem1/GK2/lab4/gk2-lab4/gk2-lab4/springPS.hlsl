#define NLIGHTS 2
SamplerState samp : register(s0);
Texture2D normTex : register(t0);

float4 lightPos[NLIGHTS];
float3 lightColor[NLIGHTS];
Texture2D albedoTex;
Texture2D roughnessTex;
Texture2D metallicTex;

TextureCube irMap;
TextureCube pfEnvMap;
Texture2D brdfTex;


static float PI = 3.14159265f;

float normalDistributionGGX(float3 N, float3 H, float r)
{
	float mx = max(dot(N, H), 0);
	float mian = mx * mx * (r * r - 1) + 1;
	mian = PI * mian * mian;
	return r * r / mian;
}

float geometrySchlickGGX(float3 N, float3 W, float r)
{
	float q = (r + 1.0f) * (r + 1.0f) / 8.0f;
	float mx = max(dot(N, W), 0);
	return mx / (mx * (1 - q) + q);
}
float geometrySmith(float3 N, float3 V, float3 L, float r)
{
	return geometrySchlickGGX(N, V, r) * geometrySchlickGGX(N, L, r);
}

float3 fresnel(float3 f0, float3 N, float3 L)
{
	float dott = dot(N, L);
	return f0 + (1.0f - f0) * pow((1.0f - max(dott, 0)), 5);
}

float CT(float3 f0, float3 N, float3 V, float3 L, float3 H, float r)
{
	float mx1 = max(dot(N, V), 0);
	float mx2 = max(dot(N, L), 0);
	return fresnel(f0, H, L) * normalDistributionGGX(N, H, r) * geometrySmith(N, V, L, r)
		/ (4 * mx1 * mx2 + 1e-3);
}
/*float4 phong(float3 worldPos, float3 norm, float3 view)
{
	view = normalize(view);
	norm = normalize(norm);
	float3 color = surfaceColor * ka; //ambient
	for (int k = 0; k < NLIGHTS; ++k)
	{
		float3 lightVec = normalize(lightPos[k].xyz - worldPos);
		float3 halfVec = normalize(view + lightVec);
		color += lightColor[k] * kd * surfaceColor * saturate(dot(norm, lightVec));//diffuse
		color += lightColor[k] * ks * pow(saturate(dot(norm, halfVec)), m);//specular
	}
	return saturate(float4(color, 1.0f));
}*/

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION0;
	float3 norm : NORMAL0;
	float3 view : VIEWVEC0;
	float2 tex : TEXCOORD0;
	float3 tangent : TANGENT0;
};

float4 main(PSInput i) : SV_TARGET
{
	float3 a = pow(albedoTex.Sample(samp, i.tex).xyz, 2.2f);
	float m = metallicTex.Sample(samp, i.tex).r;
	float r = roughnessTex.Sample(samp, i.tex).r;
	float3 n = normalize(i.norm);
	float3 v = normalize(i.view);
	float3 Iir = irMap.Sample(samp, n).rgb;
	float3 f0 = float3(0.04f, 0.04f, 0.04f);
	f0 = f0 * (1.0f - m) + a * m;

	float3 kdv = (float3(1.0f, 1.0f, 1.0f) - fresnel(f0, n/*n lub v*/, v)) * (1.0f - m);
	float3 R = reflect(-v,n);
	float3 Ii = pfEnvMap.SampleLevel(samp, R, r * 6.0f).rgb;
	float2 brdf2 = brdfTex.Sample(samp,float2(max(dot(n, v), 0.0f), r)).rg;

	float3 Id = kdv * a * Iir;

	float3 Is = Ii * (f0 * brdf2.x + brdf2.y);
	float3 Io = Id + Is;

	float3 color = float3(0.0f, 0.0f, 0.0f);
	for (int j = 0; j < NLIGHTS; ++j)
	{
		float3 li = lightPos[j].rgb - i.worldPos;
		float liLen = length(li);
		li = normalize(li);
		float3 Li = lightColor[j] * (max(dot(n, li), 0)) / (liLen * liLen);
		float3 h = normalize(v + li);

		float3 kd = (float3(1.0f, 1.0f, 1.0f) - fresnel(f0, n/*n lub v*/, li)) * (1.0f - m);

		float3 fL = a / PI;
		float3 brdf = fL * kd + CT(f0, n, v, li, h, roughnessTex.Sample(samp, i.tex).r);

		color = color + (brdf * Li);
	}
	color = color + Io;
	color = color + 0.03f * a;
	color = pow(color, 0.4545f);

	return float4(color,1.0f);
}