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
    float4 colorTint; //16
    float2 scale;
    float2 offset; //32
    float time;
    float3 camPos;
    float roughness; //48
    float3 ambient;
    int type; //64
    float3 pad;
    int lightCount; //80
    
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
    if (roughness == 1)
        return 0;
    
    float specExponent = pow(max(dot(toCam, reflection), 0), spec);
    return specExponent;
}


float3 Directional(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float3 surfaceColor, float specularScale)
{
    float3 toLight = normalize(-light.Direction);
    float3 toCam = normalize(camPos - worldPos);
    
    float diff = Diffuse(normal, toLight) * surfaceColor;
    float spec = SpecularPhong(normal, toLight, toCam, roughness) * specularScale;
    return (diff + spec) * light.Intensity * light.Color;
}

//Range based attenuation
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / light.Range * light.Range));
    return att * att;
}

float3 Point(Light light, float3 worldPos, float3 normal, float specularScale, float3 surfaceColor)
{
    float3 toLight = normalize(light.Position - worldPos);
    float3 toCam = normalize(camPos - worldPos);

	            // Calculate the light amounts
    float atten = Attenuate(light, worldPos);
    float diff = Diffuse(normal, toLight);
    float spec = SpecularPhong(normal, toLight, toCam, roughness) * specularScale;

	            // Combine
    return (diff * surfaceColor + spec * atten) * light.Intensity * light.Color;
}

Texture2D SurfaceTexture : register(t0);
Texture2D SpecularMap : register(t1);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    input.uv = input.uv * scale + offset;

    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    surfaceColor *= colorTint;
    float specularScale = SpecularMap.Sample(BasicSampler, input.uv).r;
    
    float3 totalLight = ambient * surfaceColor;

    for (int i = 0; i < 5; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += Directional(light, input.normal, input.worldPos, camPos, roughness, surfaceColor, specularScale);
                break;
            
            case LIGHT_TYPE_POINT:
	            // Combine
                totalLight += Point(light, input.worldPos, input.normal, specularScale, surfaceColor);
                break;
            
            case LIGHT_TYPE_SPOT:
                float3 toLight = normalize(light.Position - input.worldPos);
                float pixelAngle = saturate(dot(-toLight, light.Direction));
            
                float outerCosAngle = cos(light.SpotOuterAngle);
                float innerCosAngle = cos(light.SpotInnerAngle);
                float fallOff = outerCosAngle - innerCosAngle;
            
                float spotTerm = saturate((pixelAngle - outerCosAngle) / fallOff);
                totalLight += Point(light, input.worldPos, input.normal, specularScale, surfaceColor) * spotTerm;
                break;
            
        }
    }
       
    
    return float4(totalLight, 1.0f);
}
