#ifndef __GGP_TOON_SHADER__
#define __GGP_TOON_SHADER__

// Defining several different methods for toon shading (including none at all)
#define TOON_SHADER_NONE			0
#define TOON_SHADER_RAMP			1
#define TOON_SHADER_CONDITIONALS	2

float ApplyToonShadingUsingRamp(float lambert, Texture2D rampTex, SamplerState clampSample)
{
    return rampTex.Sample(clampSample, float2(lambert, 0)).r;
}

#endif