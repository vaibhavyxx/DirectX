#include "ShaderStruct.hlsli"

TextureCube SkyTexture : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel_SKY input) : SV_TARGET
{
    return SkyTexture.Sample(BasicSampler, input.sampleDir);
}