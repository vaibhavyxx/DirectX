#include "ShaderStruct.hlsli"
cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    float3 normal;
    float pad;
    float3 worldPos;
    float pad2;
    matrix worldInvTranspose;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;
    //float3 movedPos = input.localPosition + offset;
   	matrix wvp = mul(projection, mul(view, world));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    
	output.uv = input.uv;
    output.normal = mul((float3x3)worldInvTranspose, input.normal);
    output.worldPos = mul(world, float4(input.localPosition, 1)).xyz;
    return output;
}