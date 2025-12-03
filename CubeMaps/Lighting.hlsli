#ifndef __GGP_LIGHTING__
#define __GGP_LIGHTING__

#define __MAX_LIGHTS 32
#define MAX_SPECULAR_EXPONENT 256.0f
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2

#define MIN_ROUGHNESS          0.0001f
#define PI                      3.14f

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

//D(h) = a2 / (pi * (n.h)2 (a2 -1) +1)2
//Normal distribution
float D_GGX(float3 n, float3 h, float r)
{
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = r * r; 
    float a2 = max(a*a, MIN_ROUGHNESS);
    
    float denomToSquare = NdotH2 * (a2 - 1) + 1;
    return a2/ (PI * denomToSquare * denomToSquare);
}
//Responsible for roughness of the texture
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
    float r = roughness + 1;
    float k = (r * r) * 0.125f;
    float NdotV = saturate(dot(n, v));
    return NdotV / (NdotV * (1 - k) + k);
}

//Calculates specular reflectance
float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    float VdotH = saturate(dot(v, h));
    float VdotHInv = 1 - VdotH;
    float VdotHInvSq = VdotHInv * VdotHInv;
    float VdotHInv5 = VdotHInvSq * VdotHInvSq * VdotHInv;
    return f0 + (1 - f0) * VdotHInv5;
}

float3 DiffuseEnergyConserve(float diffuse, float3 F, float metalness)
{
    float3 FMetal = (1 - F) * (1 - metalness);
    return diffuse * FMetal;
}

//Takes in normal, normalized light vector, normalized view vector, roughness and specular color
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, out float3 F_out)
{
    float3 h = normalize(v + l);
    float NdotV = saturate(dot(n, v));
    float NdotL = saturate(dot(n, l));

    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    F_out = F;
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
    //float3 spec = (D * F * G) / (4 * NdotV * NdotL);
    return (D * F * G) / (4);
}

float Diffuse(float3 normal, float3 to)
{
    return saturate(dot(normal, to));
}
// OLD_WAYS
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
    float3 d = worldPos - light.Position;
    float distSq = dot(d, d);
    float rangeSq = max(light.Range * light.Range, 1e-6f);
    float att = saturate(1.0f - (distSq / rangeSq));
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
    
    return Directional(light, normal, worldPos, camPos, roughness, surfaceColor, specularScale) * atten; 
}

//PBR-------------------------------------------------------------------

float3 DirectionalPBR(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness,
float3 surfaceColor, float3 specColor, float metalness)
{
    //Finds light, camera and half vector to calculate diffusion, F, specular
    //Updates diffusion
    float3 toLight = normalize(-light.Direction);
    float3 toCam = normalize(camPos - worldPos);
    float3 h = normalize(toCam + toLight);
    float diff = Diffuse(normal, toLight);

    //float3 F = F_Schlick(toCam, h, specColor);;
    float3 F;
    float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specColor, F);

    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    return (balancedDiff * surfaceColor + spec) * light.Color * light.Intensity;
}


//Calculates the final light based on light color, attenuation and intensity
float3 PointPBR(Light light, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float3 camPos, float3 specularColor,
    float metalness)
{
    float3 toLight = normalize(light.Position - worldPos);
    float3 toCam = normalize(camPos - worldPos);
    float3 h = normalize(toCam + toLight);
    float atten = Attenuate(light, worldPos);
    
    float diff = Diffuse(normal, toLight);
    float3 F;
    float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specularColor, F);
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    return (balancedDiff * surfaceColor + spec) * (light.Color * atten) * light.Intensity;
}

float3 SpotPBR(Light light, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float3 camPos, float3 specularColor,
    float metalness)
{
    float3 toLight = normalize(light.Position - worldPos);
    float pixelAngle = saturate(dot(-toLight, light.Direction));
            
    float outerCosAngle = cos(light.SpotOuterAngle);
    float innerCosAngle = cos(light.SpotInnerAngle);
    float fallOff = outerCosAngle - innerCosAngle;
            
    float spotTerm = saturate((pixelAngle - outerCosAngle) / fallOff);
    return PointPBR(light, worldPos, normal, surfaceColor, roughness, camPos, specularColor, metalness) * spotTerm;
}

#endif