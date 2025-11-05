#include "ShaderStruct.hlsli"
#include "Lighting.hlsli"

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2
#define MAX_SPECULAR_EXPONENT       256.0f

cbuffer ExternalData : register(b0)
{
    Light light;
    float4 colorTint;       //16
    float2 scale;
    float2 offset;          //32
    float time;
    float3 camPos;
    float roughness;        //48
    float3 ambient;
    int type;               //64
    float3 pad;
    int lightCount;         //80
    
}

Texture2D SurfaceTexture	: register(t0);
SamplerState BasicSampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    /*Calculate the normalized direction to this light
    • Calculate the overall diffuse color for this light using the proper vectors, surface color, light color
    and light intensity
    • Calculate the final pixel color as a sum of the final ambient and diffuse terms
    • Return that result from the pixel shader, using 1 for alpha: float4(totalLight, 1)*/
    
    input.normal = normalize(input.normal);
    input.uv = input.uv * scale + offset;
    float3 color = float3(1.0f, 0.0f, 0.0f);
    float intensity = 1.0f;
    //float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    float3 surfaceColor = colorTint;
    
    float3 ambientTerm = ambient * colorTint;

    float3 totalLight = ambientTerm;
    float3 toLight = normalize(-light.Direction);

    float diffuse = saturate(dot(input.normal, -toLight));
    diffuse = clamp(diffuse, 0.0f, 1.0f);  
    float3 diffuseTerm = (diffuse * surfaceColor) * light.Intensity * light.Color;
    totalLight += diffuseTerm;
    
    return float4(totalLight, 1.0f);
}