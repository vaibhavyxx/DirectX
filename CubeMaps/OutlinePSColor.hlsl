struct VertexShaderInput
{
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

cbuffer externalData : register(b0)
{
    float3 Color;
}

float4 main(VertexShaderInput input) : SV_TARGET
{
    return float4(Color, 1);
}