// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL
#include "math.glsl"

mediump float InterleavedGradientNoise()
{
    mediump vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy)));
}

mediump vec2 VogelDiskSample(const int sampleIndex, const int samplesCount, const mediump float phi)
{
    mediump float r = sqrt((float(sampleIndex) + 0.5) / float(samplesCount));
    mediump float theta = float(sampleIndex) * 2.4 + phi;
    return vec2(r * cos(theta), r * sin(theta));
}

mediump float FetchTerraShadow(sampler2D shadowTex, mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float shadow = 0.0;
    mediump float phi = InterleavedGradientNoise();

    for (int i = 0; i < 4; ++i) {
      mediump vec2 offset = VogelDiskSample(i, 4, phi) * 2.0 * tsize;

      uv += offset;
      shadow += texture2D(shadowTex, uv).x;
    }

    shadow *= 0.25;

    return shadow;
}


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


mediump float AvgBlockersDepthToPenumbra(const mediump float depth, const mediump float avgBlockersDepth)
{
    mediump float penumbra = PENUMBRA_LIGHT_SIZE * (depth - avgBlockersDepth) / avgBlockersDepth;
    return clamp(80.0 * penumbra * penumbra, 0.0, 1.0);
}

mediump float Penumbra(sampler2D shadowTex, const mediump vec2 uv, const mediump vec2 tsize, const mediump float phi, const mediump float depth)
{
    mediump float avgBlockersDepth = 0.0;
    mediump float blockersCount = 0.0;

    for (int i = 0; i < PENUMBRA_FILTER_SIZE; ++i) {
        mediump vec2 offsetUV = VogelDiskSample(i, PENUMBRA_FILTER_SIZE, phi) * tsize * float(PENUMBRA_FILTER_RADIUS);
        mediump float sampleDepth = map_1(texture2D(shadowTex, uv + offsetUV).x, 0.1, 100.0);

        mediump float depthTest = clamp((depth - sampleDepth), 0.0, 1.0);
        avgBlockersDepth += depthTest * sampleDepth;
        blockersCount += depthTest;
    }

    return AvgBlockersDepthToPenumbra(depth, avgBlockersDepth / blockersCount);
}

mediump float CalcDepthShadow(sampler2D shadowTex, mediump vec4 uv, const mediump vec2 tsize)
{
  uv /= uv.w;
  uv.z = uv.z * 0.5 + 0.5;

  mediump float shadow = 0.0;
#ifndef GL_ES
  mediump float currentDepth = uv.z - 2.0 * HALF_EPSILON;
#else
  mediump float currentDepth = uv.z - 13.0 * HALF_EPSILON;
#endif
  mediump float phi = InterleavedGradientNoise();
#ifdef PENUMBRA
  mediump float penumbra = Penumbra(shadowTex, uv.xy, tsize, currentDepth);
#else
  const mediump float penumbra = 1.0;
#endif

  for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
    mediump vec2 offset = VogelDiskSample(i, PSSM_FILTER_SIZE, phi) * tsize * float(PSSM_FILTER_RADIUS) * penumbra;

    uv.xy += offset;
    mediump float depth = map_1(texture2D(shadowTex, uv.xy).x, 0.1, 100.0);
    shadow += bigger(depth, currentDepth);
  }

  shadow /= float(PSSM_FILTER_SIZE);

  return shadow;
}

#if PSSM_SPLIT_COUNT == 1
mediump float CalcPSSMShadow(const mediump float depth, const mediump vec4 lightSpacePos0, sampler2D shadowTex0, const mediump vec2 texelSize0)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowTex0, lightSpacePos0, texelSize0 * PSSM_FILTER_RADIUS0);
    else
        return 1.0;
}
#endif

#if PSSM_SPLIT_COUNT == 2
mediump float CalcPSSMShadow(const mediump float depth, \
                             const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, \
                             sampler2D shadowTex0, sampler2D shadowTex1, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowTex0, lightSpacePos0, texelSize0 * PSSM_FILTER_RADIUS0);
    else if (depth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowTex1, lightSpacePos1, texelSize1 * PSSM_FILTER_RADIUS1);
    else
        return 1.0;
}
#endif

#if PSSM_SPLIT_COUNT == 3
mediump float CalcPSSMShadow(const mediump float depth, \
                             const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, const mediump vec4 lightSpacePos2, \
                             sampler2D shadowTex0, sampler2D shadowTex1, sampler2D shadowTex2, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1, const mediump vec2 texelSize2)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowTex0, lightSpacePos0, texelSize0 * PSSM_FILTER_RADIUS0);
    else if (depth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowTex1, lightSpacePos1, texelSize1 * PSSM_FILTER_RADIUS1);
    else if (depth <= PssmSplitPoints.z)
        return CalcDepthShadow(shadowTex2, lightSpacePos2, texelSize2 * PSSM_FILTER_RADIUS2);
    else
        return 1.0;
}
#endif

#if PSSM_SPLIT_COUNT == 4
mediump float CalcPSSMShadow(const mediump float depth, \
                             const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, const mediump vec4 lightSpacePos2, const mediump vec4 lightSpacePos3, \
                             sampler2D shadowTex0, sampler2D shadowTex1, sampler2D shadowTex2, sampler2D shadowTex3, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1, const mediump vec2 texelSize2, const mediump vec2 texelSize3)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowTex0, lightSpacePos0, texelSize0 * PSSM_FILTER_RADIUS0);
    else if (depth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowTex1, lightSpacePos1, texelSize1 * PSSM_FILTER_RADIUS1);
    else if (depth <= PssmSplitPoints.z)
        return CalcDepthShadow(shadowTex2, lightSpacePos2, texelSize2 * PSSM_FILTER_RADIUS2);
    else if (depth <= PssmSplitPoints.w)
        return CalcDepthShadow(shadowTex3, lightSpacePos3, texelSize3 * PSSM_FILTER_RADIUS3);
    else
        return 1.0;
}
#endif

#if DPSM_SPLIT_COUNT == 2
mediump float CalcDPSMShadow(const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, \
                             sampler2D shadowTex0, sampler2D shadowTex1, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1)
{
    return CalcDepthShadow(shadowTex0, lightSpacePos0, texelSize0);
}
#endif

#if DPSM_SPLIT_COUNT == 1
mediump float CalcDPSMShadow(sampler2D shadowTex, mediump vec4 uv, const mediump vec2 tsize)
{
    return CalcDepthShadow(shadowTex, uv, tsize);
}
#endif

mediump float GetShadow(const highp vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES], const mediump float depth, const int light, inout int tex)
{
    mediump float shadow = 1.0;

    if (LightAttenuationArray[light].x > HALF_MAX_MINUS1) {
#if PSSM_SPLIT_COUNT == 4
#if MAX_SHADOW_TEXTURES > 3
        if (tex == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], lightSpacePosArray[1], lightSpacePosArray[2], lightSpacePosArray[3], \
                                    ShadowTex0, ShadowTex1, ShadowTex2, ShadowTex3, \
                                    ShadowTexel0, ShadowTexel1, ShadowTexel2, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 4)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[4], lightSpacePosArray[5], lightSpacePosArray[6], lightSpacePosArray[7], \
                                    ShadowTex4, ShadowTex5, ShadowTex6, ShadowTex7, \
                                    ShadowTexel4, ShadowTexel5, ShadowTexel6, ShadowTexel7);
#endif
#endif

#if PSSM_SPLIT_COUNT == 3
#if MAX_SHADOW_TEXTURES > 2
        if (tex == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], lightSpacePosArray[1], lightSpacePosArray[2], \
                                    ShadowTex0, ShadowTex1, ShadowTex2, \
                                    ShadowTexel0, ShadowTexel1, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 3)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[3], lightSpacePosArray[4], lightSpacePosArray[5], \
                                    ShadowTex3, ShadowTex4, ShadowTex5, \
                                    ShadowTexel3, ShadowTexel4, ShadowTexel5);
#endif
#endif

#if PSSM_SPLIT_COUNT == 2
#if MAX_SHADOW_TEXTURES > 1
        if (tex == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], lightSpacePosArray[1], ShadowTex0, ShadowTex1, ShadowTexel0, ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 2)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[2], lightSpacePosArray[3], ShadowTex2, ShadowTex3, ShadowTexel2, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 4)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[4], lightSpacePosArray[5], ShadowTex4, ShadowTex5, ShadowTexel4, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 6)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[6], lightSpacePosArray[7], ShadowTex6, ShadowTex7, ShadowTexel6, ShadowTexel7);
#endif
#endif

#if PSSM_SPLIT_COUNT == 1
#if MAX_SHADOW_TEXTURES > 0
        if (tex == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], ShadowTex0, ShadowTexel0);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (tex == 1)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[1], ShadowTex1, ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 2)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[2], ShadowTex2, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 3)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[3], ShadowTex3, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 4)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[4], ShadowTex4, ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 5)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[5], ShadowTex5, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 6)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[6], ShadowTex6, ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 7)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[7], ShadowTex7, ShadowTexel7);
#endif
#endif

        tex += PSSM_SPLIT_COUNT;

    }
    else if (LightSpotParamsArray[light].z > 0.0) {
#if DPSM_SPLIT_COUNT == 2
#if MAX_SHADOW_TEXTURES > 1
        if (tex == 0)
            shadow = CalcDPSMShadow(lightSpacePosArray[0], lightSpacePosArray[1], ShadowTex0, ShadowTex1, ShadowTexel0, ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 1)
            shadow = CalcDPSMShadow(lightSpacePosArray[1], lightSpacePosArray[2], ShadowTex1, ShadowTex2, ShadowTexel1, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 2)
            shadow = CalcDPSMShadow(lightSpacePosArray[2], lightSpacePosArray[3], ShadowTex2, ShadowTex3, ShadowTexel2, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 3)
            shadow = CalcDPSMShadow(lightSpacePosArray[3], lightSpacePosArray[4], ShadowTex3, ShadowTex4, ShadowTexel3, ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 4)
            shadow = CalcDPSMShadow(lightSpacePosArray[4], lightSpacePosArray[5], ShadowTex4, ShadowTex5, ShadowTexel4, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 5)
            shadow = CalcDPSMShadow(lightSpacePosArray[5], lightSpacePosArray[6], ShadowTex5, ShadowTex6, ShadowTexel5, ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 6)
            shadow = CalcDPSMShadow(lightSpacePosArray[6], lightSpacePosArray[7], ShadowTex6, ShadowTex7, ShadowTexel6, ShadowTexel7);
#endif
#endif

#if DPSM_SPLIT_COUNT == 1
#if MAX_SHADOW_TEXTURES > 0
        if (tex == 0)
            shadow = CalcDPSMShadow(ShadowTex0, lightSpacePosArray[0], ShadowTexel0);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (tex == 1)
            shadow = CalcDPSMShadow(ShadowTex1, lightSpacePosArray[1], ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 2)
            shadow = CalcDPSMShadow(ShadowTex2, lightSpacePosArray[2], ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 3)
            shadow = CalcDPSMShadow(ShadowTex3, lightSpacePosArray[3], ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 4)
            shadow = CalcDPSMShadow(ShadowTex4, lightSpacePosArray[4], ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 5)
            shadow = CalcDPSMShadow(ShadowTex5, lightSpacePosArray[5], ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 6)
            shadow = CalcDPSMShadow(ShadowTex6, lightSpacePosArray[6], ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 7)
            shadow = CalcDPSMShadow(ShadowTex7, lightSpacePosArray[7], ShadowTexel7);
#endif
#endif

        tex += DPSM_SPLIT_COUNT;

    } else {
#if MAX_SHADOW_TEXTURES > 0
        if (tex == 0)
            shadow = CalcDepthShadow(ShadowTex0, lightSpacePosArray[0], ShadowTexel0);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (tex == 1)
            shadow = CalcDepthShadow(ShadowTex1, lightSpacePosArray[1], ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 2)
            shadow = CalcDepthShadow(ShadowTex2, lightSpacePosArray[2], ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 3)
            shadow = CalcDepthShadow(ShadowTex3, lightSpacePosArray[3], ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 4)
            shadow = CalcDepthShadow(ShadowTex4, lightSpacePosArray[4], ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 5)
            shadow = CalcDepthShadow(ShadowTex5, lightSpacePosArray[5], ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 6)
            shadow = CalcDepthShadow(ShadowTex6, lightSpacePosArray[6], ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 7)
            shadow = CalcDepthShadow(ShadowTex7, lightSpacePosArray[7], ShadowTexel7);
#endif

        tex += 1;

    }

    return shadow;
}

#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
