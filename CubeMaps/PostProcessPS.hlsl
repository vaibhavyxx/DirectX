#include "ShaderStruct.hlsli"

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

float4 main(VertexToPixel_PP input) : SV_Target
{
    float4 pixelColor = Pixels.Sample(ClampSampler, input.uv);
    return pixelColor;
}