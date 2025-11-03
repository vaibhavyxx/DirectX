#include "ShaderStruct.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
	float2 scale;
    float2 offset;
	float time;
}
Texture2D SurfaceTexture		: register(t0);
Texture2D ForegroundTexture		: register(t1);
SamplerState BasicSampler		: register(s0);
SamplerState ForegroundSampler	: register(s1);
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
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

	float2 uv2 = (input.uv * offset )+ scale;    
	float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv2);
	float4 foregroundColor = ForegroundTexture.Sample(ForegroundSampler, uv2);
	return (surfaceColor + foregroundColor) * colorTint;
}