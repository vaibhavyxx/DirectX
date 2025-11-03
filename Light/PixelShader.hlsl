#include "ShaderStruct.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
	float2 scale;
    float2 offset;
	float time;
    float3 camPos;
    float roughness;
    float3 ambient;
}
Texture2D SurfaceTexture	: register(t0);
SamplerState BasicSampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    //return float4(roughness.rrr, 1);
	float2 uv2 = (input.uv * offset )+ scale;    
	float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv2);
    return float4(ambient, 1.0f) * surfaceColor * colorTint;
    return surfaceColor * colorTint;
}