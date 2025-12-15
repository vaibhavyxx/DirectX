#include "ShaderStruct.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    float lineThickness;
}
Texture2D NormalMap : register(t0);
SamplerState BasicSampler : register(s0);

VertexToPixel_Outline main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixel_Outline output;
    float3 posWorld = mul(world, float4(input.localPosition, 1.0f)).xyz;
    float3 normalWorld = normalize(mul((float3x3) world, input.normal));

    posWorld += normalWorld * 0.01f;//outlineSize;
    
    matrix vp = mul(projection, view);
    output.position = mul(vp, float4(posWorld, 1.0f));
    return output;
}