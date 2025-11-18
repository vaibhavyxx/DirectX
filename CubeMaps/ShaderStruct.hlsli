#ifndef __GGP_SHADER_STRUCTS__
#define __GGP_SHADER_STRUCTS__

struct VertexShaderInput
{
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT;
};

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT;
};
struct VertexToPixel_SKY
{
    float4 position : SV_POSITION; // XYZW position (System Value Position)
    float3 sampleDir : DIRECTION;
};
#endif