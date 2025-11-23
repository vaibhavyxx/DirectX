#ifndef __GGP_LIGHTING__
#define __GGP_LIGHTING__

#define __MAX_LIGHTS 32
#define MAX_SPECULAR_EXPONENT 256.0f
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2

#define MIN_ROUGHNESS           0.0000001f
#define PI                      3.14159265f

struct Light
{
    int Type;
    float3 Direction;
    float Range;
    float3 Position;
    float Intensity;
    float3 Color;
    float SpotInnerAngle;
    float SpotOuterAngle;
    float2 Padding;
};

float NormalDistribution_GGX(float3 n, float3 h, float alpha)
{
    float NdotH = saturate(dot(n, h));
    float NdotHSq = NdotH * NdotH;
    float a2 = max(alpha, MIN_ROUGHNESS);
    
    float denomToSq = NdotH * (a2 - 1) + 1;
    return a2 / (PI * denomToSq * denomToSq);
}
//Takes in normal, normalized light vector, normalized view vector, roughness and specular color
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0)
{
    float halfVector = normalize(v + l);
    float alpha = roughness * roughness;
    return float3(0.0f, 0.0f, 0.0f);
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
        return 0.0f;
    
    float specExponent = pow(max(dot(toCam, reflection), 0), spec);
    return specExponent;
}

float3 Directional(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float3 surfaceColor, float specularScale)
{
    float3 toLight = normalize(-light.Direction);
    float3 toCam = normalize(camPos - worldPos);
    
    float3 diff = Diffuse(normal, toLight) * surfaceColor;
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

//Calculates attenuation and then multiplies that to the lamberl value
float3 Point(Light light, float3 worldPos, float3 normal, float specularScale, float3 surfaceColor, float roughness, float3 camPos)
{
    float3 toLight = normalize(light.Position - worldPos);
    float3 toCam = normalize(camPos - worldPos);
    
    float atten = Attenuate(light, worldPos);
    
    float3 diff = Diffuse(normal, toLight);
    float spec = SpecularPhong(normal, toLight, toCam, roughness) * specularScale;
    
    return (diff * surfaceColor + spec) * light.Intensity * light.Color * atten;
    //Directional(light, normal, worldPos, camPos, roughness, surfaceColor, specularScale) * atten; 
}

#endif