#ifndef __GGP_SHADER_STRUCTS__
#define __GPP_SHADER_STRUCTS__

struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};



// VS Output / PS Input struct for basic lighting
struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : POSITION;
};

#endif