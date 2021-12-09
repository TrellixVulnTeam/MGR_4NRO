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
    matrix texMatrix0;
};

cbuffer cbTextureTransform : register(b4)
{
    matrix texMatrix1;
};

cbuffer cbCameraPosition : register(b5)
{
    float4 camPos;
};

struct VSInput
{
    float3 pos : POSITION0;
    float3 norm : NORMAL0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL0;
    float3 localPos : POSITION0;
    float3 worldPos : POSITION1;
    float4 camPos : TEXCOORD0;
};

PSInput main(VSInput i)
{
    PSInput o = (PSInput)0;

    o.localPos = i.pos;
    o.localPos.y = -0.4f; /* Water Level */

    o.pos = float4(i.pos, 1.0f);
    o.worldPos = mul(worldMatrix, o.pos).xyz;

    o.pos = mul(worldMatrix, o.pos);
    o.pos = mul(viewMatrix, o.pos);
    o.pos = mul(projMatrix, o.pos);

    o.norm = mul(worldMatrix, float4(i.norm, 0.0f)).xyz;

    o.camPos = camPos;

    return o;
}
