#include "ShaderStruct.hlsli"
#include "Lighting.hlsli"
#include "ToonShader.hlsli"

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
}

Texture2D SurfaceTexture : register(t0);
Texture2D RoughnessMap : register(t1);
Texture2D NormalMap : register(t2);
Texture2D MetalnessMap : register(t3);
Texture2D ToonRamp : register(t4);
Texture2D ToonRampSpecular : register(t5);

SamplerState BasicSampler : register(s0);
SamplerState ClampSampler : register(s1);

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.uv = input.uv * scale + offset;
 
    float3 unpackedNormal = normalize(NormalMap.Sample(BasicSampler, input.uv).xyz * 2.0f - 1.0f);
    float3 n = (input.normal);
    float3 t = normalize(input.tangent - dot(input.tangent, n) * n);
    float3 b = cross(t, n);
    float3x3 tbn = float3x3(t, b, n);
    
    float3 finalNormal = mul(tbn, unpackedNormal);
    input.normal = finalNormal;

    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb * useSurfaceMap;
    surfaceColor *= colorTint.rgb;

    float3 totalLight = float3(0.0f, 0.0f, 0.0f);
    float diffuse = 0;
    float spec = 0;
    for (int i = 0; i < 5; i++)
    {
        float3 worldPos = input.worldPos;
        float3 normal = input.normal;
        float3 toCam = normalize(camPos - worldPos);
        float3 toLight = float3(0.0f, 0.0f, 0.0f);
        float atten = 1.0f;
        float spot = 1.0f;
      
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                toLight = normalize(-light.Direction);
            break;
            
            case LIGHT_TYPE_POINT:
                toLight = normalize(light.Position - input.worldPos);
                atten = Attenuate(light, input.worldPos);
                break;
            
            case LIGHT_TYPE_SPOT:
                toLight = normalize(light.Position - input.worldPos);
                atten = Attenuate(light, input.worldPos);
                spot = pow(saturate(dot(-toLight, normalize(light.Direction))), 0.5f);
                break;
        }
        diffuse = Diffuse(input.normal, toLight);
        spec = SpecularPhong(input.normal, toLight, toCam, roughness);
        
        diffuse = ApplyToonShadingUsingRamp(diffuse, ToonRamp, ClampSampler);
        spec = ApplyToonShadingUsingRamp(spec, ToonRampSpecular, ClampSampler);
        totalLight = (diffuse * surfaceColor.rgb + spec) * light.Intensity * light.Color;
    }
    return float4(totalLight, 0.0f);
}