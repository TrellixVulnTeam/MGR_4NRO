cbuffer cbWorld : register(b0) //Vertex Shader constant buffer slot 0
{
    matrix worldMatrix;
};

cbuffer cbView : register(b1) //Vertex Shader constant buffer slot 1
{
    matrix viewMatrix;
};

cbuffer cbProj : register(b2) //Vertex Shader constant buffer slot 2
{
    matrix projMatrix;
};

cbuffer cbTextureTransform : register(b3)
{
    matrix texMatrix;
};

cbuffer cbCameraPosition : register(b5)
{
    float3 camera_position;
};

cbuffer cbLightPosition : register(b4)
{
    float3 light_position;
};

struct VSInput
{
    float3 pos : POSITION0;
    float3 normal : NORMAL0;
    float2 tex : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION0;
    float3 light_position: POSITION1;
    float3 camera_position: POSITION2;
    float3 normal : NORMAL0;
    float2 tex : TEXCOORD0;
};

PSInput main(VSInput i)
{
    PSInput o = (PSInput)0;

    o.tex = i.tex;

    o.normal = mul(worldMatrix, float4(i.normal, 0.0f)).xyz;

    o.pos = float4(i.pos, 1.0f);
    o.worldPos = mul(worldMatrix, o.pos).xyz;

    o.pos = mul(worldMatrix, o.pos);
    o.pos = mul(viewMatrix, o.pos);
    o.pos = mul(projMatrix, o.pos);

    o.light_position = light_position;
    o.camera_position = camera_position;

    return o;
}
