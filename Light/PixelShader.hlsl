#include "ShaderStruct.hlsli"
#include "Lighting.hlsli"

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2
#define MAX_SPECULAR_EXPONENT       256.0f

cbuffer ExternalData : register(b0)
{
    Light lights[5];
    //Light light;
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

//Uses lambert equation
float3 Diffuse(float3 normal, float3 to)
{
    return saturate(dot(normal, to));
}

float SpecularPhong(float3 normal, float3 dirToLight, float3 toCam, float roughness)
{
    float3 reflection = reflect(-dirToLight, normal);
    float spec = (1 - roughness) * MAX_SPECULAR_EXPONENT;
    if(roughness == 1)
        return 0;
    
    float specExponent = pow(max(dot(toCam, reflection), 0), spec);
    return specExponent;
}

float3 DirLight(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float3 surfaceColor, float specularScale)
{
    float3 toLight = normalize(-light.Direction);
    float3 toCam = normalize(camPos - worldPos);
    
    float diff = Diffuse(normal, toLight);
    float spec = SpecularPhong(normal, toLight, toCam, roughness) * specularScale;
    return (diff * surfaceColor) * light.Intensity * light.Color;
}

Texture2D SurfaceTexture	: register(t0);
Texture2D SpecularMap : register(t1);
SamplerState BasicSampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    Light thisLight = lights[lightCount];
    input.normal = normalize(input.normal);
    input.uv = input.uv * scale + offset;

    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    surfaceColor *= colorTint;
    float specularScale = SpecularMap.Sample(BasicSampler, input.uv).r;
    
    float3 ambientTerm = ambient * surfaceColor;

    float3 totalLight = ambientTerm;
    for (int i = 0; i < 2; i++)
    {
        Light light = lights[i];
        
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirLight(light, input.normal, input.worldPos, camPos, roughness, surfaceColor, specularScale);
                break;
            
            case LIGHT_TYPE_POINT:
                break;
            
            case LIGHT_TYPE_SPOT:
                break;
            
        }
    }
       
    
    return float4(totalLight, 1.0f);
}
