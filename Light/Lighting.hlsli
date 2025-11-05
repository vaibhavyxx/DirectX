#ifndef __GGP_LIGHTING__
#define __GGP_LIGHTING__

#define __MAX_LIGHTS 32
#define MAX_SPECULAR_EXPONENT 256.0f
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2

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

#endif