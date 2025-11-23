#ifndef __GGP_LIGHTING__
#define __GGP_LIGHTING__

#define __MAX_LIGHTS 32
#define MAX_SPECULAR_EXPONENT 256.0f
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2

#define MIN_ROUGHNESS           0.0000001f
#define PI                      3.14159265f
#define F0_NON_METAL            0.04f

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

float D_GGX(float3 n, float3 h, float alpha)
{
    float NdotH = saturate(dot(n, h));
    float NdotHSq = NdotH * NdotH;
    float a2 = max(alpha, MIN_ROUGHNESS);
    
    float denomToSq = NdotH * (a2 - 1) + 1;
    return a2 / (PI * denomToSq * denomToSq);
}

float G_SchlickGGX(float3 n, float3 v, float roughness)
{
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));
    return 1 / (NdotV * (1 - k) + k);
}

float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    float VdotH = saturate(dot(v, h));
    float VdotHinv = 1 - VdotH;
    return (f0 + (1 - f0) * pow(VdotHinv, 5));
}

float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    float3 Finv = 1 - F;
    return diffuse * (Finv * (1 - metalness));
}

//Takes in normal, normalized light vector, normalized view vector, roughness and specular color
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, float h)
{
    //float halfVector = normalize(v + l);
    float alpha = roughness * roughness;
    
    float D = D_GGX(n, h, alpha);
    float3 F = F_Schlick(v, h, f0);
    float g1 = G_SchlickGGX(n, v, roughness);
    float g2 = G_SchlickGGX(n, l, roughness);
    float G = g1 * g2;
    
    return (D * F * G)/4.0f;
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

float3 Directional(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, 
float3 surfaceColor, float3 specColor, float metalness)
{
    float3 toLight = normalize(-light.Direction);
    float3 toCam = normalize(camPos - worldPos);
    
    float3 diff = Diffuse(normal, toLight);
    float h = normalize(toCam + toLight);

    float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specColor, h);
    float3 F = F_Schlick(toCam, h,specColor);
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    return (balancedDiff + spec + surfaceColor) * light.Intensity * light.Color;
}

//Range based attenuation
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - ((dist * dist) / (light.Range * light.Range)));
    return att * att;
}

//Calculates attenuation and then multiplies that to the lamberl value
float3 Point(Light light, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float3 camPos, float3 specularColor,
    float metalness)
{
    float3 toLight = normalize(light.Position - worldPos);
    float3 toCam = normalize(camPos - worldPos);
    float h = normalize(toCam + toLight);
    float atten = Attenuate(light, worldPos);
    
    float3 diff = Diffuse(normal, toLight);
    float spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specularColor, h);
    float3 F = F_Schlick(toCam, h, specularColor);
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    return (balancedDiff + surfaceColor + spec) * light.Intensity * (light.Color * atten);
}

#endif