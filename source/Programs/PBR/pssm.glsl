// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL
#include "math.glsl"

#if MAX_SHADOW_TEXTURES > 0
#ifndef PSSM_SPLIT_COUNT
#define PSSM_SPLIT_COUNT 1
#endif
#ifndef PSSM_FILTER_RADIUS
#define PSSM_FILTER_RADIUS 12
#define PSSM_FILTER_RADIUS0 1.0
#define PSSM_FILTER_RADIUS1 0.3
#define PSSM_FILTER_RADIUS2 0.2
#define PSSM_FILTER_RADIUS3 0.1
#endif
#ifndef PSSM_FILTER_SIZE
#define PSSM_FILTER_SIZE 12
#endif
#ifndef PENUMBRA_FILTER_RADIUS
#define PENUMBRA_FILTER_RADIUS 12
#endif
#ifndef PENUMBRA_FILTER_SIZE
#define PENUMBRA_FILTER_SIZE 12
#endif
#ifndef PENUMBRA_LIGHT_SIZE
#define PENUMBRA_LIGHT_SIZE 50.0
#endif

float InterleavedGradientNoise()
{
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy)));
}

vec2 VogelDiskSample(const int sampleIndex, const int samplesCount, const float phi)
{
    float r = sqrt((float(sampleIndex) + 0.5) / float(samplesCount));
    float theta = float(sampleIndex) * 2.4 + phi;
    return vec2(r * cos(theta), r * sin(theta));
}

#ifdef TERRA_NORMALMAP
float FetchTerraShadow(sampler2D shadowTex, vec2 uv)
{
    float shadow = 0.0;
    float phi = InterleavedGradientNoise();
    vec2 tsize = 1.0 / vec2(textureSize(shadowTex, 0));

    for (int i = 0; i < 4; ++i) {
      vec2 offset = VogelDiskSample(i, 4, phi) * 2.0 * tsize;

      uv += offset;
      shadow += texture2D(shadowTex, uv).x;
    }

    shadow *= 0.25;

    return shadow;
}
#endif

float AvgBlockersDepthToPenumbra(const float depth, const float avgBlockersDepth)
{
    float penumbra = PENUMBRA_LIGHT_SIZE * (depth - avgBlockersDepth) / avgBlockersDepth;
    return clamp(80.0 * penumbra * penumbra, 0.0, 1.0);
}

float Penumbra(sampler2D shadowTex, const vec2 uv, const vec2 tsize, const float phi, const float depth)
{
    float avgBlockersDepth = 0.0;
    float blockersCount = 0.0;

    for (int i = 0; i < PENUMBRA_FILTER_SIZE; ++i) {
        vec2 offsetUV = VogelDiskSample(i, PENUMBRA_FILTER_SIZE, phi) * tsize * float(PENUMBRA_FILTER_RADIUS);
        float sampleDepth = map_1(texture2D(shadowTex, uv + offsetUV).x, 0.1, 100.0);

        float depthTest = clamp((depth - sampleDepth), 0.0, 1.0);
        avgBlockersDepth += depthTest * sampleDepth;
        blockersCount += depthTest;
    }

    return AvgBlockersDepthToPenumbra(depth, avgBlockersDepth / blockersCount);
}

float CalcDepthShadow(sampler2D shadowTex, vec4 uv)
{
    uv /= uv.w;
    uv.z = uv.z * 0.5 + 0.5;
    vec2 tsize = 1.0 / vec2(textureSize(shadowTex, 0));
    float shadow = 0.0;
    float currentDepth = uv.z - 2.0 * HALF_EPSILON;
    float phi = InterleavedGradientNoise();
#ifdef PENUMBRA
    float penumbra = Penumbra(shadowTex, uv.xy, tsize, currentDepth);
#else
    const float penumbra = 1.0;
#endif

    for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
        vec2 offset = VogelDiskSample(i, PSSM_FILTER_SIZE, phi) * tsize * float(PSSM_FILTER_RADIUS) * penumbra;

        uv.xy += offset;
        float depth = map_1(texture2D(shadowTex, uv.xy).x, 0.1, 100.0);
        shadow += bigger(depth, currentDepth);
    }

    shadow /= float(PSSM_FILTER_SIZE);

    return shadow;
}

#if PSSM_SPLIT_COUNT > 0
float CalcPSSMShadow(const vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES]) {
    float depth = gl_FragCoord.z / gl_FragCoord.w;

    if (depth <= PssmSplitPoints.x) return CalcDepthShadow(ShadowTex0, lightSpacePosArray[0]);
#if PSSM_SPLIT_COUNT > 1
    else if (depth <= PssmSplitPoints.y) return CalcDepthShadow(ShadowTex1, lightSpacePosArray[1]);
#endif
#if PSSM_SPLIT_COUNT > 2
    else if (depth <= PssmSplitPoints.z) return CalcDepthShadow(ShadowTex2, lightSpacePosArray[2]);
#endif
#if PSSM_SPLIT_COUNT > 3
    else if (depth <= PssmSplitPoints.w) return CalcDepthShadow(ShadowTex3, lightSpacePosArray[3]);
#endif
    else return 1.0;
}
#endif


float CalcShadow(const vec4 lightSpacePos, const int index)
{
#if MAX_SHADOW_TEXTURES > 0
    if (index == 0) return CalcDepthShadow(ShadowTex0, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 1
    else if (index == 1) return CalcDepthShadow(ShadowTex1, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 2
    else if (index == 2) return CalcDepthShadow(ShadowTex2, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 3
    else if (index == 3) return CalcDepthShadow(ShadowTex3, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 4
    else if (index == 4) return CalcDepthShadow(ShadowTex4, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 5
    else if (index == 5) return CalcDepthShadow(ShadowTex5, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 6
    else if (index == 6) return CalcDepthShadow(ShadowTex6, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 7
    else if (index == 7) return CalcDepthShadow(ShadowTex7, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 0
    else return 1.0;
#endif
}

#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
