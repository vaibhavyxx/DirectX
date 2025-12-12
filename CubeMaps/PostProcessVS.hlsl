#include "ShaderStruct.hlsli"

VertexToPixel_PP main(uint id : SV_VertexID)
{
    VertexToPixel_PP output;

    //Does bitwise operations to calculate the vertices of a triangle
    output.uv = float2((id << 1) & 2, id & 2);
    output.position = float4(output.uv, 0, 1);
    output.position.x = output.position.x * 2 - 1;
    output.position.y = output.position.y * -2 + 1;
    
    return output;
}