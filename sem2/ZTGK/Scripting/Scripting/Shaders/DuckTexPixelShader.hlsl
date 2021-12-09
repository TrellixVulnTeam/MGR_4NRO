Texture2D textureMap;
sampler samp;

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION0;
    float3 light_position: POSITION1;
    float3 camera_position: POSITION2;
    float3 normal : NORMAL0;
    float2 tex : TEXCOORD0;
};

static const float3 ambientColor = float3(0.4f, 0.4f, 0.4f);
static const float3 lightColor = float3(1.0f, 1.0f, 1.0f);
static const float kd = 0.5f, ks = 0.5f, m = 50;

float4 main(PSInput i) : SV_TARGET
{
    float3 tex_color = textureMap.Sample(samp, i.tex).xyz;

    float3 view_vector = normalize(i.camera_position - i.worldPos);
    float3 light_vector = normalize(i.light_position - i.worldPos);

    float3 normal = normalize(i.normal);
    float3 result_color = tex_color * ambientColor; /* Ambient Colour */

    result_color += lightColor * tex_color * kd * dot(normal, light_vector); /* Diffuse Colour */

    float3 T = float3(normal.x, -normal.z, normal.y);
    float3 half_vector = normalize(view_vector + light_vector);    

    float anisotropic = sqrt(1 - pow(dot(T, half_vector), 2)); /* Anisotropic Colour */
    result_color += lightColor * tex_color * ks * pow(anisotropic, m); /* Specular - Anisotropic Colour */

    return float4(saturate(result_color), 1.0f);
}
