#include "ShaderStruct.hlsli"

Texture2D SurfaceTexture : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel_SKY input) : SV_Target
{
    return SurfaceTexture.Sample(BasicSampler, input.sampleDir);
}
