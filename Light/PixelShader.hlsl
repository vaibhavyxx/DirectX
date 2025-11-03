#include "ShaderStruct.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
	float2 scale;
    float2 offset;
	float time;
    float3 camPos;
    float roughness;
}
Texture2D SurfaceTexture	: register(t0);
SamplerState BasicSampler	: register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return float4(roughness.rrr, 1);
	
	float2 uv2 = (input.uv * offset )+ scale;    
	float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv2);
	return surfaceColor * colorTint;
}