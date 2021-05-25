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

cbuffer cbLighting : register(b1) //Pixel Shader constant buffer slot 1
{
	Lighting lighting;
}

float4 main(PSInput i) : SV_TARGET
{
	float3 surfaceColor = duckTex.Sample(samp, i.tex).rgb;
	float3 V = normalize(i.view);
	float3 N = normalize(i.norm);
	float3 T = normalize(cross(N,float3(0.0f,1.0f,0.0f)));
	T = normalize(cross(T, N));
	float specAlpha = 0.0f;
	float3 col = lighting.ambient.xyz * lighting.surface.x;
	for (int k = 0; k < 1; ++k)
	{
		Light li = lighting.lights[k];
		if (li.color.w != 0)
		{
			float3 L = normalize(li.position.xyz - i.worldPos);
			float3 H = normalize(V + L);
			col += pow(surfaceColor.xyz * clamp(dot(N, L), 0.0f, 1.0f),0.8);
			float nh = dot(T, H);
			nh = sqrt(1.0f - nh * nh);
			nh = clamp(nh, 0.0f, 1.0f);
			nh = pow(nh, lighting.surface.w);
			//specAlpha += nh;
			col += li.color.xyz * nh;
		}
	}
	return float4(col, 1.0f);
}