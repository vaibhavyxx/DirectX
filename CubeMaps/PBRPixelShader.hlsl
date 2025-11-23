#include "ShaderStruct.hlsli"
#include "Lighting.hlsli"

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2
#define MAX_SPECULAR_EXPONENT       256.0f

cbuffer ExternalData : register(b0)
{
    Light lights[5];
    float4 colorTint;   //16
    float2 scale;
    float2 offset;      //32
    float time;
    float3 camPos;      //48
    float roughness; 
    int type; 
    int lightCount; 
    bool flat;          //80
}
Texture2D Albedo : register(t0);
Texture2D RoughnessMap : register(t1);
Texture2D NormalMap : register(t2);
Texture2D MetalnessMap : register(t3);

SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.uv = input.uv * scale + offset;
    
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    
    float3 unpackedNormal = normalize(NormalMap.Sample(BasicSampler, input.uv).xyz * 2.0f - 1.0f);
    float3 n = normalize(input.normal);
    float3 t = normalize(input.tangent - dot(input.tangent, n) * n);
    float3 b = cross(t, n);
    float3x3 tbn = float3x3(t, b, n);
    
    float3 finalNormal = mul(tbn, unpackedNormal);
    input.normal = finalNormal;
    
    float3 albedo = float3(1.0f, 1.0f, 1.0f);
    if (!flat)
    {
        albedo = Albedo.Sample(BasicSampler, input.uv).rgb;
    }
    albedo *= colorTint.rgb;
    albedo = pow(albedo, 2.2f);
    //float specularScale = SpecularMap.Sample(BasicSampler, input.uv).r;
    float3 specularColor = lerp(0.04f, albedo, metalness);
    float3 totalLight = albedo;

    for (int i = 0; i < 5; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += Directional(light, input.normal, input.worldPos, camPos, roughness, albedo, specularScale);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += Point(light, input.worldPos, input.normal, specularScale, albedo, roughness, camPos);
                break;
            
            case LIGHT_TYPE_SPOT:
                float3 toLight = normalize(light.Position - input.worldPos);
                float pixelAngle = saturate(dot(-toLight, light.Direction));
            
                float outerCosAngle = cos(light.SpotOuterAngle);
                float innerCosAngle = cos(light.SpotInnerAngle);
                float fallOff = outerCosAngle - innerCosAngle;
            
                float spotTerm = saturate((pixelAngle - outerCosAngle) / fallOff);
                totalLight += Point(light, input.worldPos, input.normal, specularScale, albedo, roughness, camPos) * spotTerm;
                break;
            
        }
    }
    float3 gammaAdjusted = totalLight;
    if (!flat)
    {
        gammaAdjusted = pow(gammaAdjusted, 1.0f / 2.2f);
    }
    else
    {
        gammaAdjusted = pow(gammaAdjusted, 2.2f);
    }
    
    return float4(gammaAdjusted, 1.0f);
}
