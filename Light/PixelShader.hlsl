#include "ShaderStruct.hlsli"

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2
#define MAX_SPECULAR_EXPONENT       256.0f

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float2 scale;
    float2 offset;
    float time;
    float3 camPos;
    float roughness;
    float3 ambient;
    int type;
    float3 pad;
}

Texture2D SurfaceTexture	: register(t0);
SamplerState BasicSampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    //return float4(light.Color, 1);
    
    input.normal = normalize(input.normal);
    return float4(input.normal, 1);
    
    //return float4(roughness.rrr, 1);
	float2 uv2 = (input.uv * offset )+ scale;    
	float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv2);
    return float4(ambient, 1.0f) * surfaceColor * colorTint;
    return surfaceColor * colorTint;
}