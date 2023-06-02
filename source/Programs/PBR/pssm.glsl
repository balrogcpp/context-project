// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL
#include "math.glsl"

mediump float InterleavedGradientNoise()
{
    mediump vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy)));
}

mediump vec2 VogelDiskSample(const int sampleIndex, const mediump float samplesCount, const mediump float phi)
{
    mediump float r = sqrt((float(sampleIndex) + 0.5) / samplesCount);
    mediump float theta = float(sampleIndex) * 2.4 + phi;
   return vec2(r * cos(theta), r * sin(theta));
}

mediump float FetchTerraShadow(sampler2D shadowMap, mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float shadow = 0.0;
    mediump float phi = InterleavedGradientNoise();

    for (int i = 0; i < 4; ++i) {
      mediump vec2 offset = VogelDiskSample(i, 4.0, phi) * 2.0 * tsize;

      uv += offset;
      shadow += texture2D(shadowMap, uv.xy).x;
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


mediump float AvgBlockersDepthToPenumbra(const mediump float z_shadowMapView, const mediump float avgBlockersDepth)
{
    mediump float penumbra = PENUMBRA_LIGHT_SIZE * (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
    return clamp(80.0 * penumbra * penumbra, 0.0, 1.0);
}

mediump float map_01(const mediump float x)
{
    return x * 100.0;
}

mediump float Penumbra(sampler2D shadowMap, const mediump vec2 lightSpace, const mediump vec2 tsize, const mediump float phi, const mediump float z_shadowMapView)
{
    mediump float avgBlockersDepth = 0.0;
    mediump float blockersCount = 0.0;

    for (int i = 0; i < PENUMBRA_FILTER_SIZE; ++i) {
        mediump vec2 offsetUV = VogelDiskSample(i, float(PENUMBRA_FILTER_SIZE), phi) * tsize * float(PENUMBRA_FILTER_RADIUS);
        mediump float sampleDepth = map_01(texture2D(shadowMap, lightSpace + offsetUV).x);

        mediump float depthTest = clamp((z_shadowMapView - sampleDepth), 0.0, 1.0);
        avgBlockersDepth += depthTest * sampleDepth;
        blockersCount += depthTest;
    }

    return AvgBlockersDepthToPenumbra(z_shadowMapView, avgBlockersDepth / blockersCount);
}

mediump float CalcDepthShadow(sampler2D shadowMap, mediump vec4 lightSpace, const mediump vec2 tsize)
{
  lightSpace /= lightSpace.w;
  lightSpace.z = lightSpace.z * 0.5 + 0.5;

  mediump float shadow = 0.0;
  mediump float currentDepth = lightSpace.z - 2.0 * HALF_EPSILON;
  mediump float phi = InterleavedGradientNoise();
#ifdef PENUMBRA
  mediump float penumbra = Penumbra(shadowMap, lightSpace.xy, tsize, currentDepth);
#else
  const mediump float penumbra = 1.0;
#endif

  for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
    mediump vec2 offset = VogelDiskSample(i, float(PSSM_FILTER_SIZE), phi) * tsize * float(PSSM_FILTER_RADIUS) * penumbra;

    lightSpace.xy += offset;
    mediump float depth = map_01(texture2D(shadowMap, lightSpace.xy).x);
    shadow += bigger(depth, currentDepth);
  }

  shadow /= float(PSSM_FILTER_SIZE);

  return shadow;
}

#if PSSM_SPLIT_COUNT == 1
mediump float CalcPSSMShadow(const mediump float depth, const mediump vec4 lightSpacePos0, sampler2D shadowMap0, const mediump vec2 texelSize0)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0 * PSSM_FILTER_RADIUS0);
    else
        return 1.0;
}
#endif

#if PSSM_SPLIT_COUNT == 2
mediump float CalcPSSMShadow(const mediump float depth, \
                             const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, \
                             sampler2D shadowMap0, sampler2D shadowMap1, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0 * PSSM_FILTER_RADIUS0);
    else if (depth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowMap1, lightSpacePos1, texelSize1 * PSSM_FILTER_RADIUS1);
    else
        return 1.0;
}
#endif

#if PSSM_SPLIT_COUNT == 3
mediump float CalcPSSMShadow(const mediump float depth, \
                             const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, const mediump vec4 lightSpacePos2, \
                             sampler2D shadowMap0, sampler2D shadowMap1, sampler2D shadowMap2, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1, const mediump vec2 texelSize2)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0 * PSSM_FILTER_RADIUS0);
    else if (depth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowMap1, lightSpacePos1, texelSize1 * PSSM_FILTER_RADIUS1);
    else if (depth <= PssmSplitPoints.z)
        return CalcDepthShadow(shadowMap2, lightSpacePos2, texelSize2 * PSSM_FILTER_RADIUS2);
    else
        return 1.0;
}
#endif

#if PSSM_SPLIT_COUNT == 4
mediump float CalcPSSMShadow(const mediump float depth, \
                             const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, const mediump vec4 lightSpacePos2, const mediump vec4 lightSpacePos3, \
                             sampler2D shadowMap0, sampler2D shadowMap1, sampler2D shadowMap2, sampler2D shadowMap3, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1, const mediump vec2 texelSize2, const mediump vec2 texelSize3)
{
    if (depth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0 * PSSM_FILTER_RADIUS0);
    else if (depth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowMap1, lightSpacePos1, texelSize1 * PSSM_FILTER_RADIUS1);
    else if (depth <= PssmSplitPoints.z)
        return CalcDepthShadow(shadowMap2, lightSpacePos2, texelSize2 * PSSM_FILTER_RADIUS2);
    else if (depth <= PssmSplitPoints.w)
        return CalcDepthShadow(shadowMap3, lightSpacePos3, texelSize3 * PSSM_FILTER_RADIUS3);
    else
        return 1.0;
}
#endif

#if DPSM_SPLIT_COUNT == 2
mediump float CalcDPSMShadow(const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, \
                             sampler2D shadowMap0, sampler2D shadowMap1, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1)
{
    return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0);
}
#endif

#if DPSM_SPLIT_COUNT == 1
mediump float CalcDPSMShadow(sampler2D shadowMap, mediump vec4 lightSpace, const mediump vec2 tsize)
{
    return CalcDepthShadow(shadowMap, lightSpace, tsize);
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
                                    ShadowMap0, ShadowMap1, ShadowMap2, ShadowMap3, \
                                    ShadowTexel0, ShadowTexel1, ShadowTexel2, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 4)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[4], lightSpacePosArray[5], lightSpacePosArray[6], lightSpacePosArray[7], \
                                    ShadowMap4, ShadowMap5, ShadowMap6, ShadowMap7, \
                                    ShadowTexel4, ShadowTexel5, ShadowTexel6, ShadowTexel7);
#endif
#endif

#if PSSM_SPLIT_COUNT == 3
#if MAX_SHADOW_TEXTURES > 2
        if (tex == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], lightSpacePosArray[1], lightSpacePosArray[2], \
                                    ShadowMap0, ShadowMap1, ShadowMap2, \
                                    ShadowTexel0, ShadowTexel1, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 3)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[3], lightSpacePosArray[4], lightSpacePosArray[5], \
                                    ShadowMap3, ShadowMap4, ShadowMap5, \
                                    ShadowTexel3, ShadowTexel4, ShadowTexel5);
#endif
#endif

#if PSSM_SPLIT_COUNT == 2
#if MAX_SHADOW_TEXTURES > 1
        if (tex == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], lightSpacePosArray[1], ShadowMap0, ShadowMap1, ShadowTexel0, ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 2)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[2], lightSpacePosArray[3], ShadowMap2, ShadowMap3, ShadowTexel2, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 4)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[4], lightSpacePosArray[5], ShadowMap4, ShadowMap5, ShadowTexel4, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 6)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[6], lightSpacePosArray[7], ShadowMap6, ShadowMap7, ShadowTexel6, ShadowTexel7);
#endif
#endif

#if PSSM_SPLIT_COUNT == 1
#if MAX_SHADOW_TEXTURES > 0
        if (tex == 0)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[0], ShadowMap0, ShadowTexel0);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (tex == 1)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[1], ShadowMap1, ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 2)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[2], ShadowMap2, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 3)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[3], ShadowMap3, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 4)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[4], ShadowMap4, ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 5)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[5], ShadowMap5, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 6)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[6], ShadowMap6, ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 7)
            shadow = CalcPSSMShadow(depth, lightSpacePosArray[7], ShadowMap7, ShadowTexel7);
#endif
#endif

        tex += PSSM_SPLIT_COUNT;

    }
    else if (LightSpotParamsArray[light].z > 0.0) {
#if DPSM_SPLIT_COUNT == 2
#if MAX_SHADOW_TEXTURES > 1
        if (tex == 0)
            shadow = CalcDPSMShadow(lightSpacePosArray[0], lightSpacePosArray[1], ShadowMap0, ShadowMap1, ShadowTexel0, ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 1)
            shadow = CalcDPSMShadow(lightSpacePosArray[1], lightSpacePosArray[2], ShadowMap1, ShadowMap2, ShadowTexel1, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 2)
            shadow = CalcDPSMShadow(lightSpacePosArray[2], lightSpacePosArray[3], ShadowMap2, ShadowMap3, ShadowTexel2, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 3)
            shadow = CalcDPSMShadow(lightSpacePosArray[3], lightSpacePosArray[4], ShadowMap3, ShadowMap4, ShadowTexel3, ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 4)
            shadow = CalcDPSMShadow(lightSpacePosArray[4], lightSpacePosArray[5], ShadowMap4, ShadowMap5, ShadowTexel4, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 5)
            shadow = CalcDPSMShadow(lightSpacePosArray[5], lightSpacePosArray[6], ShadowMap5, ShadowMap6, ShadowTexel5, ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 6)
            shadow = CalcDPSMShadow(lightSpacePosArray[6], lightSpacePosArray[7], ShadowMap6, ShadowMap7, ShadowTexel6, ShadowTexel7);
#endif
#endif

#if DPSM_SPLIT_COUNT == 1
#if MAX_SHADOW_TEXTURES > 0
        if (tex == 0)
            shadow = CalcDPSMShadow(ShadowMap0, lightSpacePosArray[0], ShadowTexel0);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (tex == 1)
            shadow = CalcDPSMShadow(ShadowMap1, lightSpacePosArray[1], ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 2)
            shadow = CalcDPSMShadow(ShadowMap2, lightSpacePosArray[2], ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 3)
            shadow = CalcDPSMShadow(ShadowMap3, lightSpacePosArray[3], ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 4)
            shadow = CalcDPSMShadow(ShadowMap4, lightSpacePosArray[4], ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 5)
            shadow = CalcDPSMShadow(ShadowMap5, lightSpacePosArray[5], ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 6)
            shadow = CalcDPSMShadow(ShadowMap6, lightSpacePosArray[6], ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 7)
            shadow = CalcDPSMShadow(ShadowMap7, lightSpacePosArray[7], ShadowTexel7);
#endif
#endif

        tex += DPSM_SPLIT_COUNT;

    } else {
#if MAX_SHADOW_TEXTURES > 0
        if (tex == 0)
            shadow = CalcDepthShadow(ShadowMap0, lightSpacePosArray[0], ShadowTexel0);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (tex == 1)
            shadow = CalcDepthShadow(ShadowMap1, lightSpacePosArray[1], ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 2)
            shadow = CalcDepthShadow(ShadowMap2, lightSpacePosArray[2], ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 3)
            shadow = CalcDepthShadow(ShadowMap3, lightSpacePosArray[3], ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 4)
            shadow = CalcDepthShadow(ShadowMap4, lightSpacePosArray[4], ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 5)
            shadow = CalcDepthShadow(ShadowMap5, lightSpacePosArray[5], ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 6)
            shadow = CalcDepthShadow(ShadowMap6, lightSpacePosArray[6], ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 7)
            shadow = CalcDepthShadow(ShadowMap7, lightSpacePosArray[7], ShadowTexel7);
#endif

        tex += 1;

    }

    return shadow;
}

#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
