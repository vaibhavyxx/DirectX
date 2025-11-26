#include "ShaderStruct.hlsli"
#include "Lighting.hlsli"

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2
#define MAX_SPECULAR_EXPONENT       256.0f

cbuffer ExternalData : register(b0)
{
    Light lights[5];
    
    float4 colorTint; //16
    
    float2 scale;
    float2 offset; //32
    
    float time;
    float3 camPos;
    
    float roughness; //48
    float3 ambient;
    
    int type; 
    int lightCount; 
    int useGamma;
    int useNormal;
}
Texture2D SurfaceTexture : register(t0);
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
    float metal = MetalnessMap.Sample(BasicSampler, input.uv).r;
    //metal = 0.0f;
    //roughness = 0.5f;
    
    float3 unpackedNormal = normalize(NormalMap.Sample(BasicSampler, input.uv).xyz * 2.0f -1.0f);
    float3 n = (input.normal);
    float3 t = normalize(input.tangent - dot(input.tangent, n) * n);
    float3 b = cross(t, n);
    float3x3 tbn = float3x3(t, b, n);
    
    float3 finalNormal = mul(tbn, unpackedNormal);
    input.normal = finalNormal;
 
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    //surfaceColor = pow(surfaceColor, 2.2f);
    float3 dielectricF0 = float3(0.04, 0.04, 0.04);
    float3 specularColor = lerp(dielectricF0, surfaceColor, metal);
    float3 totalLight = ambient * surfaceColor;
    
    for (int i = 0; i < 5; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionalPBR(light, input.normal, input.worldPos, camPos, roughness, surfaceColor, specularColor, metal);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += PointPBR(light, input.worldPos, input.normal, surfaceColor, roughness, camPos,specularColor ,metal);
                break;
            
            case LIGHT_TYPE_SPOT:
                totalLight += SpotPBR(light, input.worldPos, input.normal, surfaceColor, roughness, camPos, specularColor, metal);
                break;
        }
    }
    totalLight = pow(totalLight, 0.45f);
    return float4(totalLight, 1.0f);
}
