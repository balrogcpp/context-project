// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform float ChromaticRadius;
uniform int FeaturesCount;

vec3 SampleChromatic(sampler2D tex, const vec2 uv, const float radius)
{
    vec2 offset = normalize(vec2(0.5, 0.5) - uv) * radius;

    vec3 color = vec3(
        texture2D(tex, uv + offset).r,
        texture2D(tex, uv).g,
        texture2D(tex, uv - offset).b
    );

    return color;
}

// https://john-chapman.github.io/2017/11/05/pseudo-lens-flare.html
// Cubic window; map [0, radius] in [1, 0] as a cubic falloff from center.
float WindowCubic(const float x, const float center, const float radius)
{
    float y = min(abs(x - center) / radius, 1.0);
    return 1.0 - y * y * (3.0 - 2.0 * y);
}

vec3 GhostFeatures(sampler2D tex, const vec2 uv, const vec2 texel, const int counter, const float radius)
{
    vec2 nuv = vec2(1.0, 1.0) - uv;
    vec2 ghostVec = (vec2(0.5, 0.5) - nuv) * 0.44;
    vec3 color = vec3(0.0, 0.0, 0.0);

    #define MAX_GHOST_COUNT 24

    // ghosts
    for (int i = 0; i < MAX_GHOST_COUNT; ++i) {
        if (counter <= i) break;

        vec2 suv = fract(nuv + ghostVec * float(i));
        float d = distance(suv, vec2(0.5, 0.5));
        float w = pow(1.0 - d, 5.0) / float(counter);
        vec3 s = SampleChromatic(tex, suv, radius);
        color += s * w;
    }

    return color;
}

vec3 HaloFeatures(sampler2D tex, const vec2 uv, const vec2 texel, const int counter, const float radius)
{
    vec2 nuv = vec2(1.0, 1.0) - uv;
    vec2 haloVec = vec2(0.5, 0.5) - nuv;

    #define RADIUS 0.45

    // halo
    float ratio = texel.y / texel.x;
    haloVec.x *= ratio;
    haloVec = normalize(haloVec);
    haloVec.x /= ratio;
    vec2 wuv = (nuv - vec2(0.5, 0.0)) / vec2(ratio, 1.0) + vec2(0.5, 0.0);
    float d = distance(wuv, vec2(0.5, 0.5));
    float w = pow(1.0 - d, 5.0);
    haloVec *= RADIUS;

    vec2 suv = nuv + haloVec;
    vec3 s = SampleChromatic(tex, suv, radius);
    vec3 color = s * w;

    return color;
}

in vec2 vUV0;
void main()
{
    vec2 TexelSize0 = 1.0 / vec2(textureSize(RT, 0));
    vec3 color = texture2D(RT, vUV0).rgb;

    //color += GhostFeatures(RT, vUV0, TexelSize0, FeaturesCount, ChromaticRadius) * 0.5;
    color += HaloFeatures(RT, vUV0, TexelSize0, FeaturesCount, ChromaticRadius);
    FragColor.rgb = color;
}
