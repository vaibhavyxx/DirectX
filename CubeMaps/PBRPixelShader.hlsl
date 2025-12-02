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
    bool useGamma;
    
    bool useNormals;
    bool useRoughness;
    bool useMetals;
    bool useAldedo;     //96
}
Texture2D Albedo : register(t0);
Texture2D RoughnessMap : register(t1);
Texture2D NormalMap : register(t2);
Texture2D MetalnessMap : register(t3);

SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float3 nrm = normalize(input.normal);
    float3 tgt = normalize(input.tangent);
    float2 uv = input.uv * scale + offset;
    
    float roughness = RoughnessMap.Sample(BasicSampler, uv).r;
    roughness = useRoughness ? roughness : 0.1f;    //default value
    
    float metalness = MetalnessMap.Sample(BasicSampler, uv).r;
    float3 unpackedNormal = normalize(NormalMap.Sample(BasicSampler, uv).xyz * 2.0f - 1.0f);
    float3 t = tgt - dot(tgt, nrm) * nrm;
    float3 b = cross(t, nrm);
    float3x3 tbn = float3x3(t, b, nrm);
    
    float3 finalNormal = mul(tbn, unpackedNormal);
    input.normal = finalNormal;

    input.normal = useNormals? finalNormal: input.normal;
    input.tangent = tgt;
    input.uv = uv;

    float3 surfaceColor = Albedo.Sample(BasicSampler, input.uv).rgb;
    surfaceColor = pow(surfaceColor, 2.2f);

    surfaceColor = useGamma ? pow(surfaceColor, 2.2f) : surfaceColor.rgb; 
    surfaceColor = useAldedo ? surfaceColor : colorTint.rgb;
    
    float3 specularColor = lerp(0.04f, surfaceColor, metalness);
    float3 totalLight = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < lightCount; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        float3 worldPos = input.worldPos;
        float3 normal = input.normal;
        
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionalPBR(light, normal, worldPos, camPos, roughness, surfaceColor, specularColor, metalness);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += PointPBR(light, worldPos, normal, surfaceColor, roughness, camPos, specularColor, metalness);
                break;
            
            case LIGHT_TYPE_SPOT:
                totalLight += SpotPBR(light, worldPos, normal, surfaceColor, roughness, camPos, specularColor, metalness);
                break;
        }
    }
    totalLight = useGamma ? pow(totalLight, 1.0f / 2.2f):totalLight;
    return float4(totalLight, 1.0f);
}
