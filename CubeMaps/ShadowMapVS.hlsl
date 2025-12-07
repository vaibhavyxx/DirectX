#include "ShaderStruct.hlsli"

// Constant Buffer for external (C++) data
cbuffer externalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// --------------------------------------------------------
float4 main(VertexShaderInput input) : SV_POSITION
{
    VertexToPixel output;
    matrix wvp = mul(projection, mul(view, world));
    output.shadowMapPos = mul(wvp, float4(input.localPosition, 1.0f));
    return mul(wvp, float4(input.localPosition, 1.0f));
    //return 0;
}