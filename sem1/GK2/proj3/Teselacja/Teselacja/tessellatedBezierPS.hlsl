Texture2D diffuseTex : register(t0);
texture2D normalMap  : register(t1);
SamplerState colorSampler : register(s0);

struct Light
{
	float4 position;
	float4 color;
};

struct Lighting
{
	float4 ambient;
	float4 surface;
	Light lights[3];
};

struct Parameters2
{
	int colorTexture;
	int normalMapping;
	int dummy1;
	int dummy2;
};

cbuffer cbSurfaceColor : register(b0)
{
	float4 surfaceColor;
};

cbuffer cbLighting : register(b1) //Pixel Shader constant buffer slot 1
{
	Lighting lighting;
}

cbuffer cbParameters : register(b2)
{
	Parameters2 parameters2;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 tangent: TANGENT;
	float3 normal: NORMAL;
	float3 binormal: BINORMAL;
	float3 worldPos : POSITION;
	float3 view : VIEW;
	float2 tex : TEXCOORD;
};

float4 main(PSInput i) : SV_TARGET
{
	float3 V = normalize(i.view);
	float3 N = normalize(i.normal);
	
	float3 norm = 2.0f * (normalMap.Sample(colorSampler, i.tex).rgb - 0.5f);
	if (parameters2.normalMapping == 1)

	{
		N = normalize(norm.x * i.tangent + norm.y * i.binormal + norm.z * i.normal);
	}

	float3 col = lighting.ambient.xyz * lighting.surface.x;
	if (parameters2.colorTexture == 1)
	{
		col = diffuseTex.Sample(colorSampler, i.tex).rgb;
	}

	float specAlpha = 0.0f;
	for (int k = 0; k < 1; ++k)
	{
		Light li = lighting.lights[k];
		if (li.color.w != 0)
		{
			float3 L = normalize(li.position.xyz - i.worldPos);
			float3 H = normalize(V + L);
			col += li.color.xyz * surfaceColor.xyz * lighting.surface.y * clamp(dot(N, L), 0.0f, 1.0f);
			float nh = dot(N, H);
			nh = clamp(nh, 0.0f, 1.0f);
			nh = pow(nh, lighting.surface.w);
			specAlpha += nh;
			col += li.color.xyz * nh;
		}
	}

	return saturate(float4(col, surfaceColor.w + specAlpha));
}