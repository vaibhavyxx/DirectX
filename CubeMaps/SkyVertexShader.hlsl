#include "ShaderStruct.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
}


VertexToPixel_SKY main(VertexShaderInput input)
{
    VertexToPixel_SKY output;
    
    matrix viewNoTranslation = view;
    viewNoTranslation._14 = 0.0f;
    viewNoTranslation._24 = 0.0f;
    viewNoTranslation._34 = 0.0f;
    
    matrix vp = mul(projection, viewNoTranslation);
    output.position = mul(vp, float4(input.localPosition, 1.0f));
    output.position.z = output.position.w;
    output.sampleDir = input.localPosition;
    
    return output;
}