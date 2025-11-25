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

float D_GGX(float3 n, float3 h, float r)
{
    float NdotH = saturate(dot(n, h));
    float NdotHSq = NdotH * NdotH;
    float a1 = r * r;
    float a2 = max(a1, MIN_ROUGHNESS);
    
    float denomToSq = NdotH * (a2 - 1) + 1;
    return a2 / (PI * denomToSq * denomToSq);
}

float G_SchlickGGX(float3 n, float3 v, float roughness)
{
    float r = roughness + 1;
    float k = (r * r) * 0.125f;
    //pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));
    return 1 / (NdotV * (1 - k) + k);
}

float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    float VdotH = saturate(dot(v, h));
    float VdotHinv = 1 - VdotH;
    float VdotH2 = VdotH * VdotH;
    float VdotH5 = VdotH2 * VdotH2 * VdotH;
    return (f0 + (1 - f0) * VdotH5);
}

float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    //float3 Finv = 1 - F;
    diffuse *= (1 - F);
    diffuse *= metalness;
    return diffuse;// * (1 - F) * (1 - metalness);
}

//Takes in normal, normalized light vector, normalized view vector, roughness and specular color
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, out float3 F_out)
{
    float3 h = normalize(v + l);
    //float alpha = roughness * roughness;
    //float NdotV = saturate(dot(n, v));
    float NdotL = saturate(dot(n, l));
    
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float g1 = G_SchlickGGX(n, v, roughness);
    float g2 = G_SchlickGGX(n, l, roughness);
    float G = g1 * g2;
    F_out = F;
    
    //Cook Torrance: (D*G*F)/(4 * NdotV *NdotL)
    float3 specularResult = (D * F * G) *0.25f;
    
    return specularResult * max(NdotL, 0);
}

//Uses lambert equation
float3 Diffuse(float3 normal, float3 to)
{
    return (saturate(dot(normal, to)) / PI);
}

float SpecularPhong(float3 normal, float3 dirToLight, float3 toCam, float roughness)
{
    float3 reflection = reflect(-dirToLight, normal);
    float spec = (1 - roughness) * MAX_SPECULAR_EXPONENT;
    //spec *= (1 - roughness);
    
    float specExponent = pow(max(dot(toCam, reflection), 0), spec);
    return specExponent;
}

float3 Directional(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, 
float3 surfaceColor, float3 specColor, float metalness)
{
    float3 toLight = normalize(-light.Direction);
    float3 toCam = normalize(camPos - worldPos);
    
    float3 diff = Diffuse(normal, toLight);
    //float3 h = normalize(toCam + toLight);
    
    float3 F; // = F_Schlick(toCam, h, specColor);
    float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specColor,F);
    
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    return (balancedDiff + spec + surfaceColor) * light.Color * light.Intensity; //Removing light intensity
}

//Range based attenuation
float Attenuate(Light light, float3 worldPos)
{
    float3 diff = light.Position - worldPos;
    float dist = sqrt(diff);
    float att = saturate(1.0f - ((dist) / (light.Range * light.Range)));
    return att * att;
}

//Calculates attenuation and then multiplies that to the lamberl value
float3 Point(Light light, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float3 camPos, float3 specularColor,
    float metalness)
{
    float3 toLight = normalize(light.Position - worldPos);
    float3 toCam = normalize(camPos - worldPos);
    //float3 h = normalize(toCam + toLight);
    float atten = Attenuate(light, worldPos);
    
    float3 diff = Diffuse(normal, toLight);
    float3 F;// = F_Schlick(toCam, h, specularColor);
    float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specularColor, F);
    //float3 F = F_Schlick(toCam, h, specularColor);
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    return (balancedDiff + surfaceColor + spec) * (light.Color * atten) * light.Intensity;    //Removing light intensity
}

float3 Spot(Light light, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float3 camPos, float3 specularColor,
    float metalness)
{
    float3 toLight = normalize(light.Position - worldPos);
    float pixelAngle = saturate(dot(-toLight, light.Direction));
            
    float outerCosAngle = cos(light.SpotOuterAngle);
    float innerCosAngle = cos(light.SpotInnerAngle);
    float fallOff = outerCosAngle - innerCosAngle;
            
    float spotTerm = saturate((pixelAngle - outerCosAngle) / fallOff);
    return Point(light, worldPos, normal, surfaceColor, roughness, camPos, specularColor, metalness) * spotTerm;
}
#endif