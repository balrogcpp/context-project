// created by Andrey Vasiliev

#ifndef REFLECTION_GLSL
#define REFLECTION_GLSL

#include "noise.glsl"


//----------------------------------------------------------------------------------------------------------------------
vec4 GetProjectionCoord(const vec4 position) {
    return mat4(0.5, 0.0, 0.0, 0.0,
                0.0, 0.5, 0.0, 0.0,
                0.0, 0.0, 0.5, 0.0,
                0.5, 0.5, 0.5, 1.0
                ) * position;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 ApplyReflection(const vec3 color, const vec3 n, const vec3 v, const float metallic)
{
    vec4 projection = projectionCoord;

    const float fresnelBias = 0.1;
    const float fresnelScale = 1.8;
    const float fresnelPower = 8.0;
    const float filter_max_size = 0.1;
    const int sample_count = 8;

    float cosa = dot(n, -v);
    float fresnel = fresnelBias + fresnelScale * pow(1.0 + cosa, fresnelPower);
    fresnel = clamp(fresnel, 0.0, 1.0);
    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    projection.xy += VogelDiskSample(3, sample_count, gradientNoise) * filter_max_size;
    vec3 reflectionColour = texture2DProj(uReflectionMap, projection).rgb;
    return mix(color, reflectionColour, fresnel * metallic);
}

#endif
