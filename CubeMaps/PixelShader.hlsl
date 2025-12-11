#include "ShaderStruct.hlsli"
#include "Lighting.hlsli"

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2
#define MAX_SPECULAR_EXPONENT       256.0f

cbuffer ExternalData : register(b0)
{
    Light lights[5]; //16
    float4 colorTint; //32
    float2 scale;
    float2 offset; //48
    float time;
    float3 camPos; //64
    float roughness;
    int type;
    int lightCount;
    int useGamma; //80
    int useNormals;
    int useRoughness;
    int useMetals;
    int useSurfaceMap; //96
    bool usePBR;
    float3 pad;
}
Texture2D SurfaceTexture : register(t0);
Texture2D RoughnessMap : register(t1);
Texture2D NormalMap : register(t2);
Texture2D MetalnessMap : register(t3);
Texture2D ShadowMap : register(t4);

SamplerState BasicSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.uv = input.uv * scale + offset;

    float roughValue = RoughnessMap.Sample(BasicSampler, input.uv).r * useRoughness;
    if (useRoughness == 0)
        roughValue = 0.2f;
    
    float metal = MetalnessMap.Sample(BasicSampler, input.uv).r * useMetals;
    if (useMetals == 0)
        metal = 0.0f;
   
    float3 unpackedNormal = normalize(NormalMap.Sample(BasicSampler, input.uv).xyz * 2.0f - 1.0f);
    float3 n = (input.normal);
    float3 t = normalize(input.tangent - dot(input.tangent, n) * n);
    float3 b = cross(t, n);
    float3x3 tbn = float3x3(t, b, n);
    
    float3 finalNormal = mul(unpackedNormal, tbn);
    input.normal = finalNormal;

    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb * useSurfaceMap;
    if (useSurfaceMap == 0)
        surfaceColor = colorTint.rgb;
    
    float3 dielectricF0 = float3(0.04f, 0.04f, 0.04f);
    float3 specularColor = lerp(dielectricF0, surfaceColor, metal);

    input.shadowMapPos /= input.shadowMapPos.w;
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
    float distToLight = input.shadowMapPos.z;
    float depthFromLight = input.shadowMapPos.z / input.shadowMapPos.w;
    float distShadowMap = ShadowMap.Sample(BasicSampler, shadowUV).r;

    //Using this test code to show that objects do render both shadows and themselves
    //Currently issues with implementing shadows that turns everything black
    //if (distShadowMap < distToLight)
    //    return float4(0, 0, 0, 1);
    float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
	
    float3 totalLight = float3(0.0f, 0.0f, 0.0f);
    if (useGamma == 1)
        surfaceColor = pow(surfaceColor, 2.2f);
    
    for (int i = 0; i < lightCount; i++)
    {
        float3 worldPos = input.worldPos;
        float3 normal = input.normal;
        
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                //if (usePBR)
                float3 dirLight = DirectionalPBR(light, normal, worldPos, camPos, roughValue, surfaceColor, specularColor, metal);
                totalLight += (dirLight);// * shadowAmount); commented out as it breaks
                break;
            
            case LIGHT_TYPE_POINT:
                if (usePBR)
                    totalLight += PointPBR(light, worldPos, normal, surfaceColor, roughValue, camPos, specularColor, metal);
           
                break;
            
            case LIGHT_TYPE_SPOT:
            //if(usePBR)
             float3 spotLight = SpotPBR(light, worldPos, normal, surfaceColor, roughness, camPos, specularColor, metal);
                totalLight += (spotLight);// * shadowAmount);   commented out as it breaks
                break;
        }
    }
    if (useGamma)
        surfaceColor = pow(surfaceColor, 0.45f);
    return float4(totalLight, 1.0f);
}
