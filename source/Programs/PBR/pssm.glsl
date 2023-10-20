// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL
#include "math.glsl"

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

#if MAX_SHADOW_TEXTURES > 0
#ifndef PSSM_SPLIT_COUNT
    #define PSSM_SPLIT_COUNT 2
#endif
#ifndef DPSM_SPLIT_COUNT
    #define DPSM_SPLIT_COUNT 2
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
#ifndef GL_ES
  float currentDepth = uv.z - 2.0 * HALF_EPSILON;
#else
  float currentDepth = uv.z - 13.0 * HALF_EPSILON;
#endif
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

#if PSSM_SPLIT_COUNT == 1
float CalcPSSMShadow(const float depth, const vec4 lightSpacePos0, sampler2D shadowTex0, const vec2 texelSize0)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowTex0, lightSpacePos0, texelSize0 * PSSM_FILTER_RADIUS0);
    else
        return 1.0;
}
#endif

#if PSSM_SPLIT_COUNT == 2
float CalcPSSMShadow(const float depth, \
                             const vec4 lightSpacePos0, const vec4 lightSpacePos1, \
                             sampler2D shadowTex0, sampler2D shadowTex1)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowTex0, lightSpacePos0);
    else if (depth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowTex1, lightSpacePos1);
    else
        return 1.0;
}
#endif

#if PSSM_SPLIT_COUNT == 3
float CalcPSSMShadow(const float depth, \
                             const vec4 lightSpacePos0, const vec4 lightSpacePos1, const vec4 lightSpacePos2, \
                             sampler2D shadowTex0, sampler2D shadowTex1, sampler2D shadowTex2)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowTex0, lightSpacePos0);
    else if (depth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowTex1, lightSpacePos1);
    else if (depth <= PssmSplitPoints.z)
        return CalcDepthShadow(shadowTex2, lightSpacePos2);
    else
        return 1.0;
}
#endif

#if PSSM_SPLIT_COUNT == 4
float CalcPSSMShadow(const float depth, \
                             const vec4 lightSpacePos0, const vec4 lightSpacePos1, const vec4 lightSpacePos2, const vec4 lightSpacePos3, \
                             sampler2D shadowTex0, sampler2D shadowTex1, sampler2D shadowTex2, sampler2D shadowTex3) {
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowTex0, lightSpacePos0);
    else if (depth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowTex1, lightSpacePos1);
    else if (depth <= PssmSplitPoints.z)
        return CalcDepthShadow(shadowTex2, lightSpacePos2);
    else if (depth <= PssmSplitPoints.w)
        return CalcDepthShadow(shadowTex3, lightSpacePos3);
    else
        return 1.0;
}
#endif

#if DPSM_SPLIT_COUNT == 2
float CalcDPSMShadow(const vec4 lightSpacePos0, const vec4 lightSpacePos1, \
                             sampler2D shadowTex0, sampler2D shadowTex1)
{
    return CalcDepthShadow(shadowTex0, lightSpacePos0);
}
#endif

#if DPSM_SPLIT_COUNT == 1
float CalcDPSMShadow(sampler2D shadowTex, vec4 uv)
{
    return CalcDepthShadow(shadowTex, uv);
}
#endif

int texCounter = 0;
float GetShadow(const highp vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES], const float depth, const int light)
{
    float shadow = 1.0;

    if (LightAttenuationArray[light].x > HALF_MAX_MINUS1) {
#if PSSM_SPLIT_COUNT == 4
#if MAX_SHADOW_TEXTURES > 3
        if (texCounter == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], lightSpacePosArray[1], lightSpacePosArray[2], lightSpacePosArray[3], \
                                    ShadowTex0, ShadowTex1, ShadowTex2, ShadowTex3);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (texCounter == 4)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[4], lightSpacePosArray[5], lightSpacePosArray[6], lightSpacePosArray[7], \
                                    ShadowTex4, ShadowTex5, ShadowTex6, ShadowTex7);
#endif
#endif

#if PSSM_SPLIT_COUNT == 3
#if MAX_SHADOW_TEXTURES > 2
        if (texCounter == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], lightSpacePosArray[1], lightSpacePosArray[2], \
                                    ShadowTex0, ShadowTex1, ShadowTex2);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (texCounter == 3)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[3], lightSpacePosArray[4], lightSpacePosArray[5], \
                                    ShadowTex3, ShadowTex4, ShadowTex5);
#endif
#endif

#if PSSM_SPLIT_COUNT == 2
#if MAX_SHADOW_TEXTURES > 1
        if (texCounter == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], lightSpacePosArray[1], ShadowTex0, ShadowTex1);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (texCounter == 2)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[2], lightSpacePosArray[3], ShadowTex2, ShadowTex3);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (texCounter == 4)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[4], lightSpacePosArray[5], ShadowTex4, ShadowTex5);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (texCounter == 6)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[6], lightSpacePosArray[7], ShadowTex6, ShadowTex7);
#endif
#endif

#if PSSM_SPLIT_COUNT == 1
#if MAX_SHADOW_TEXTURES > 0
        if (texCounter == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], ShadowTex0);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (texCounter == 1)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[1], ShadowTex1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (texCounter == 2)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[2], ShadowTex2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (texCounter == 3)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[3], ShadowTex3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (texCounter == 4)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[4], ShadowTex4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (texCounter == 5)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[5], ShadowTex5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (texCounter == 6)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[6], ShadowTex6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (texCounter == 7)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[7], ShadowTex7);
#endif
#endif

        texCounter += PSSM_SPLIT_COUNT;

    }
    else if (LightSpotParamsArray[light].z > 0.0) {
#if DPSM_SPLIT_COUNT == 2
#if MAX_SHADOW_TEXTURES > 1
        if (texCounter == 0)
            shadow = CalcDPSMShadow(lightSpacePosArray[0], lightSpacePosArray[1], ShadowTex0, ShadowTex1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (texCounter == 1)
            shadow = CalcDPSMShadow(lightSpacePosArray[1], lightSpacePosArray[2], ShadowTex1, ShadowTex2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (texCounter == 2)
            shadow = CalcDPSMShadow(lightSpacePosArray[2], lightSpacePosArray[3], ShadowTex2, ShadowTex3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (texCounter == 3)
            shadow = CalcDPSMShadow(lightSpacePosArray[3], lightSpacePosArray[4], ShadowTex3, ShadowTex4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (texCounter == 4)
            shadow = CalcDPSMShadow(lightSpacePosArray[4], lightSpacePosArray[5], ShadowTex4, ShadowTex5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (texCounter == 5)
            shadow = CalcDPSMShadow(lightSpacePosArray[5], lightSpacePosArray[6], ShadowTex5, ShadowTex6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (texCounter == 6)
            shadow = CalcDPSMShadow(lightSpacePosArray[6], lightSpacePosArray[7], ShadowTex6, ShadowTex7);
#endif
#endif

#if DPSM_SPLIT_COUNT == 1
#if MAX_SHADOW_TEXTURES > 0
        if (texCounter == 0)
            shadow = CalcDPSMShadow(ShadowTex0, lightSpacePosArray[0]);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (texCounter == 1)
            shadow = CalcDPSMShadow(ShadowTex1, lightSpacePosArray[1]);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (texCounter == 2)
            shadow = CalcDPSMShadow(ShadowTex2, lightSpacePosArray[2]);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (texCounter == 3)
            shadow = CalcDPSMShadow(ShadowTex3, lightSpacePosArray[3]);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (texCounter == 4)
            shadow = CalcDPSMShadow(ShadowTex4, lightSpacePosArray[4]);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (texCounter == 5)
            shadow = CalcDPSMShadow(ShadowTex5, lightSpacePosArray[5]);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (texCounter == 6)
            shadow = CalcDPSMShadow(ShadowTex6, lightSpacePosArray[6]);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (texCounter == 7)
            shadow = CalcDPSMShadow(ShadowTex7, lightSpacePosArray[7]);
#endif
#endif

        texCounter += DPSM_SPLIT_COUNT;

    } else {
#if MAX_SHADOW_TEXTURES > 0
        if (texCounter == 0)
            shadow = CalcDepthShadow(ShadowTex0, lightSpacePosArray[0]);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (texCounter == 1)
            shadow = CalcDepthShadow(ShadowTex1, lightSpacePosArray[1]);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (texCounter == 2)
            shadow = CalcDepthShadow(ShadowTex2, lightSpacePosArray[2]);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (texCounter == 3)
            shadow = CalcDepthShadow(ShadowTex3, lightSpacePosArray[3]);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (texCounter == 4)
            shadow = CalcDepthShadow(ShadowTex4, lightSpacePosArray[4]);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (texCounter == 5)
            shadow = CalcDepthShadow(ShadowTex5, lightSpacePosArray[5]);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (texCounter == 6)
            shadow = CalcDepthShadow(ShadowTex6, lightSpacePosArray[6]);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (texCounter == 7)
            shadow = CalcDepthShadow(ShadowTex7, lightSpacePosArray[7]);
#endif

        texCounter += 1;

    }

    return shadow;
}

#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
