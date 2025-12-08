#include "ShaderStruct.hlsli"

// Constant Buffer for external (C++) data
cbuffer externalData : register(b0)
{
    matrix world;
    matrix lightView;
    matrix lightProjection;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// --------------------------------------------------------
float4 main(VertexShaderInput input) : SV_POSITION
{
    //VertexToPixel output;
    matrix wvp = mul(lightProjection, mul(lightView, world));
    return mul(wvp, float4(input.localPosition, 1.0f));
    //return 0;
}