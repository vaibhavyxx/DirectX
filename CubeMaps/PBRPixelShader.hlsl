#include "ShaderStruct.hlsli"
#include "Lighting.hlsli"

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2
#define MAX_SPECULAR_EXPONENT       256.0f

cbuffer ExternalData : register(b0)
{
    Light lights[2];
    float4 colorTint;   //16
    
    float2 scale;
    float2 offset;      //32
    
    float time;
    float3 camPos;      //48
    
    float roughness; 
    int type; 
    int lightCount; 
    
    bool useGamma;      //80
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
    //input.normal = normalize(input.normal);
    float3 nrm = normalize(input.normal);
    float3 tgt = normalize(input.tangent);
    float2 uv = input.uv * scale + offset;
    
    float roughness = RoughnessMap.Sample(BasicSampler, uv).r;
    float3 rough = RoughnessMap.Sample(BasicSampler, uv).rgb;
    roughness = useRoughness ? roughness : 0.1f;    //default value
    
    float metalness = MetalnessMap.Sample(BasicSampler, uv).r;
    metalness = 1.0f;
    //useMetals ? metalness : 0.0f;  -- issue here
    
    //return float4(rough.r, 0,0, 0);
    
    float3 unpackedNormal = normalize(NormalMap.Sample(BasicSampler, uv).xyz * 2.0f - 1.0f);
    float3 t = tgt - dot(tgt, nrm) * nrm;
    float3 b = cross(t, nrm);
    float3x3 tbn = float3x3(t, b, nrm);
    
    float3 finalNormal = mul(tbn, unpackedNormal);
    input.normal = finalNormal;
    input.normal = useNormals? finalNormal: input.normal;
    input.tangent = tgt;
    input.uv = uv;

    float3 albedo = Albedo.Sample(BasicSampler, input.uv).rgb;
    albedo = pow(albedo, 2.2f);
    //albedo *= colorTint.rgb;
    //return (metalness, 0, 0, .0f);
    
    //false - metals
    //normals - false
    //roughness - false
    //albedo = useGamma ? pow(albedo, 2.2f) : albedo.rgb; //false
    //albedo = useAldedo ? albedo : colorTint.rgb; //- bug
    
    float3 specularColor = lerp(0.04f, albedo, metalness);
    float3 totalLight = albedo;

    for (int i = 0; i < 2; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += Directional(light, input.normal, input.worldPos, camPos, roughness, albedo, specularColor, metalness);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += Point(light, input.worldPos, input.normal, albedo, roughness, camPos, specularColor, metalness);
                break;
            
            case LIGHT_TYPE_SPOT:
                totalLight += Spot(light, input.worldPos, input.normal, albedo, roughness, camPos, specularColor, metalness);
                break;
            
        }
    }
    totalLight = pow(totalLight, 0.45f);
    
    //float3 gammaAdjusted = useGamma ? pow(totalLight, 1.0f / 2.2f):totalLight;
    return float4(totalLight, 1.0f);
}
