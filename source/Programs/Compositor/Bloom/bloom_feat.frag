// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "math.glsl"
#include "srgb.glsl"


//----------------------------------------------------------------------------------------------------------------------
vec3 SampleChromatic(const sampler2D tex, const vec2 uv, const float radius)
{
    vec2 offset = normalize(vec2(0.5) - uv) * radius;

    vec3 color =  vec3(
        texture2D(tex, uv + offset).r,
        texture2D(tex, uv).g,
        texture2D(tex, uv - offset).b
    );

    color /= (1.0 + luminance(LINEARtoSRGB(color)));
    return color;
}


// https://john-chapman.github.io/2017/11/05/pseudo-lens-flare.html
// Cubic window; map [0, radius] in [1, 0] as a cubic falloff from center.
float Window_Cubic(const float x, const float center, const float radius)
{
    float y = min(abs(x - center) / radius, 1.0);
    return 1.0 - y * y * (3.0 - 2.0 * y);
}


//----------------------------------------------------------------------------------------------------------------------
vec3 GhostFeatures(const sampler2D tex, const vec2 _uv, const vec2 texel, const int counter, const float radius)
{
    vec2 uv = vec2(1.0) - _uv;
    vec2 ghostVec = (vec2(0.5) - uv) * 0.44;
    vec3 color = vec3(0.0);

    #define MAX_GHOST_COUNT 24

    // ghosts
    for (int i = 0; i < MAX_GHOST_COUNT; ++i) {
        if (counter <= i) break;

        vec2 suv = fract(uv + ghostVec * float(i));
        float d = distance(suv, vec2(0.5));
        float w = pow(1.0 - d, 5.0) / float(counter);
        vec3 s = SampleChromatic(tex, suv, radius);
        color += s * w;
    }

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 HaloFeatures(const sampler2D tex, const vec2 _uv, const vec2 texel, const int counter, const float radius)
{
    vec2 uv = vec2(1.0) - _uv;
    vec2 haloVec = vec2(0.5) - uv;

    #define RADIUS 0.45

    // halo
    float ratio = texel.y / texel.x;
    haloVec.x *= ratio;
    haloVec = normalize(haloVec);
    haloVec.x /= ratio;
    vec2 wuv = (uv - vec2(0.5, 0.0)) / vec2(ratio, 1.0) + vec2(0.5, 0.0);
    float d = distance(wuv, vec2(0.5));
    float w = pow(1.0 - d, 5.0);
    haloVec *= RADIUS;

    vec2 suv = uv + haloVec;
    vec3 s = SampleChromatic(tex, suv, radius);
    vec3 color = s * w;

    return color;
}


in vec2 vUV0;
uniform sampler2D ColorMap;
uniform vec2 TexelSize0;
uniform float ChromaticRadius;
uniform int FeaturesCount;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    vec3 color = texture2D(ColorMap, vUV0).rgb;
    color += GhostFeatures(ColorMap, vUV0, TexelSize0, FeaturesCount, ChromaticRadius);
    color += HaloFeatures(ColorMap, vUV0, TexelSize0, FeaturesCount, ChromaticRadius);

    FragColor.rgb = color;
}
