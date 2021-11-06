#include "LightHelper.fx"

#define MAX_BONE_COUNT 67
#define LIGHT_SIZE 2

cbuffer cbPerObject
{
	matrix worldMatrix;
	matrix boneMatrix[MAX_BONE_COUNT];
	Material mat;
};

cbuffer cbPerFrame
{
	matrix viewMatrix;
	float3 eyePos;
};

cbuffer cbPerScene
{
	DirectionalLight dirLight[LIGHT_SIZE];
	matrix projMatrix;
};

struct VSInput
{
	float3 pos : POSITION;
	float3 norm : NORMAL0;
	float3 weights : WEIGHTS;
	uint4 boneIndices : BONEINDICES;
};

struct PSInput
{
	float4 pos: SV_POSITION;
	float3 worldPos: POSITION0;
	float3 norm: NORMAL0;
};

//Vertex Shader
PSInput VS(VSInput i)
{
	//TODO: 13. Uzupełnić Vertex Shader
	PSInput o;

	float weights[4] = { 0.0f,0.0f,0.0f,0.0f };
	weights[0] = i.weights.x;
	weights[1] = i.weights.y;
	weights[2] = i.weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 pos = float3(0.0f, 0.0f, 0.0f);
	float3 norm = float3(0.0f, 0.0f, 0.0f);

	for (int k = 0; k < 4; ++k)
	{
		pos += weights[k] * mul(float4(i.pos, 1.0f), boneMatrix[i.boneIndices[k]]).xyz;
		norm += weights[k] * mul(float4(i.norm, 0.0f), boneMatrix[i.boneIndices[k]]).xyz;
	}

	
	o.worldPos = mul(float4(pos, 1.0f), worldMatrix).xyz;
	o.pos = mul(float4(o.worldPos, 1.0f), viewMatrix);
	o.pos = mul(o.pos,projMatrix);
	
	o.norm = mul(float4(norm,0.0f),worldMatrix).xyz;
	o.norm = normalize(o.norm);
	return o;
}

//Pixel Shader
float4 PS(PSInput i) : SV_TARGET
{
	float3 toEye = normalize(eyePos - i.worldPos);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;

	for (int index = 0; index < LIGHT_SIZE; index++)
	{
		ComputeDirectionalLight(mat, dirLight[index], i.norm, toEye, A, D, S);
		ambient += A;
		diffuse += D;
		spec += S;
	}

	float4 result_color = ambient + diffuse + spec;

	result_color.a = mat.Diffuse.a;

	return result_color;
}


technique11 SkinningTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


