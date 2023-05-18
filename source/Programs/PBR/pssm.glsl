// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL
//#define PENUMBRA

#ifndef PSSM_SPLIT_COUNT
    #define PSSM_SPLIT_COUNT 1
    #define PSSM_FILTER_RADIUS 16
    #define PSSM_FILTER_SIZE 32
    #define PENUMBRA_FILTER_RADIUS 4
    #define PENUMBRA_FILTER_SIZE 4
    #define PENUMBRA_LIGHT_SIZE 50.0
#endif


// shadow receiver
#if MAX_SHADOW_TEXTURES > 0
varying highp vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
uniform mediump vec4 ShadowDepthRangeArray[MAX_SHADOW_TEXTURES];
uniform mediump float LightCastsShadowsArray[MAX_LIGHTS];
uniform sampler2D ShadowMap0;
uniform mediump vec2 ShadowTexel0;
#if MAX_SHADOW_TEXTURES > 1
uniform sampler2D ShadowMap1;
uniform mediump vec2 ShadowTexel1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform sampler2D ShadowMap2;
uniform mediump vec2 ShadowTexel2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform sampler2D ShadowMap3;
uniform mediump vec2 ShadowTexel3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform sampler2D ShadowMap4;
uniform mediump vec2 ShadowTexel4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform sampler2D ShadowMap5;
uniform mediump vec2 ShadowTexel5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform sampler2D ShadowMap6;
uniform mediump vec2 ShadowTexel6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform sampler2D ShadowMap7;
uniform mediump vec2 ShadowTexel7;
#endif
#if MAX_SHADOW_TEXTURES > 8
uniform sampler2D ShadowMap8;
uniform mediump vec2 ShadowTexel8;
#endif
uniform mediump vec4 PssmSplitPoints;
uniform mediump vec4 ShadowColour;
#endif // MAX_SHADOW_TEXTURES > 0

#include "math.glsl"


//----------------------------------------------------------------------------------------------------------------------
highp float InterleavedGradientNoise(const highp vec2 position_screen)
{
    highp vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec2 VogelDiskSample(const mediump float sampleIndex, const mediump float samplesCount, const mediump float phi)
{
    mediump float r = sqrt((sampleIndex + 0.5) / samplesCount);
    mediump float theta = sampleIndex * 2.4 + phi;
    return vec2(r * cos(theta), r * sin(theta));
}


//----------------------------------------------------------------------------------------------------------------------
mediump float AvgBlockersDepthToPenumbra(const mediump float z_shadowMapView, const mediump float avgBlockersDepth)
{
    mediump float penumbra = PENUMBRA_LIGHT_SIZE * (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
    return clamp(80.0 * penumbra * penumbra, 0.0, 1.0);
}


//----------------------------------------------------------------------------------------------------------------------
mediump float map_01(const mediump float x)
{
    return x * 100.0;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Penumbra(const sampler2D shadowMap, const mediump vec2 lightSpace, const mediump vec2 tsize, const mediump float gradientNoise, const mediump float z_shadowMapView)
{
    mediump float avgBlockersDepth = 0.0;
    mediump float blockersCount = 0.0;

    for (int i = 0; i < PENUMBRA_FILTER_SIZE; ++i) {
        mediump vec2 offsetUV = VogelDiskSample(float(i), PENUMBRA_FILTER_SIZE, gradientNoise) * tsize * PENUMBRA_FILTER_RADIUS;
        mediump float sampleDepth = map_01(texture2D(shadowMap, lightSpace + offsetUV).x);

        mediump float depthTest = clamp((z_shadowMapView - sampleDepth), 0.0, 1.0);
        avgBlockersDepth += depthTest * sampleDepth;
        blockersCount += depthTest;
    }

    return AvgBlockersDepthToPenumbra(z_shadowMapView, avgBlockersDepth / blockersCount);
}


//----------------------------------------------------------------------------------------------------------------------
mediump float FetchTerraShadow(const sampler2D shadowMap, mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float shadow = 0.0;
    mediump float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);

    for (int i = 0; i < 4; ++i) {
      mediump vec2 offset = VogelDiskSample(float(i), 4.0, gradientNoise) * 2.0 * tsize;

      uv += offset;
      shadow += texture2D(shadowMap, uv.xy).x;
    }

    shadow *= 0.25;

    return shadow;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float CalcDepthShadow(const sampler2D shadowMap, mediump vec4 lightSpace, const mediump vec2 tsize)
{
  lightSpace /= lightSpace.w;
  lightSpace.z = lightSpace.z * 0.5 + 0.5;

  mediump float shadow = 0.0;
  mediump float currentDepth = lightSpace.z - 2.0 * HALF_EPSILON;
  mediump float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
#ifdef PENUMBRA
  mediump float penumbra = Penumbra(shadowMap, lightSpace.xy, tsize, gradientNoise, currentDepth);
#else
  const mediump float penumbra = 1.0;
#endif

  for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
    mediump vec2 offset = VogelDiskSample(float(i), PSSM_FILTER_SIZE, gradientNoise) * tsize * PSSM_FILTER_RADIUS * penumbra;

    lightSpace.xy += offset;
    mediump float depth = map_01(texture2D(shadowMap, lightSpace.xy).x);
    shadow += bigger(depth, currentDepth);
  }

  shadow /= PSSM_FILTER_SIZE;

  return shadow;
}


//----------------------------------------------------------------------------------------------------------------------
#if PSSM_SPLIT_COUNT == 1
mediump float CalcPSSMShadow(const sampler2D shadowMap0, const mediump vec4 lightSpacePos0, const mediump vec2 texelSize0)
{
    if (vScreenPosition.z <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0);
    else
        return 0.0;
}
#endif


//----------------------------------------------------------------------------------------------------------------------
#if PSSM_SPLIT_COUNT == 2
mediump float CalcPSSMShadow(const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, \
                             const sampler2D shadowMap0, const sampler2D shadowMap1, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1)
{
    if (vScreenPosition.z <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0);
    else if (vScreenPosition.z <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowMap1, lightSpacePos1, texelSize1 * 0.3);
    else
        return 0.0;
}
#endif


//----------------------------------------------------------------------------------------------------------------------
#if PSSM_SPLIT_COUNT == 3
mediump float CalcPSSMShadow(const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, const mediump vec4 lightSpacePos2, \
                             const sampler2D shadowMap0, const sampler2D shadowMap1, const sampler2D shadowMap2, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1, const mediump vec2 texelSize2)
{
    if (vScreenPosition.z <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0);
    else if (vScreenPosition.z <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowMap1, lightSpacePos1, texelSize1 * 0.3);
    else if (vScreenPosition.z <= PssmSplitPoints.z)
        return CalcDepthShadow(shadowMap2, lightSpacePos2, texelSize2 * 0.2);
    else
        return 0.0;
}
#endif


//----------------------------------------------------------------------------------------------------------------------
#if PSSM_SPLIT_COUNT == 4
mediump float CalcPSSMShadow(const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, const mediump vec4 lightSpacePos2, const mediump vec4 lightSpacePos3, \
                             const sampler2D shadowMap0, const sampler2D shadowMap1, const sampler2D shadowMap2, const sampler2D shadowMap3, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1, const mediump vec2 texelSize2, const mediump vec2 texelSize3)
{
    if (vScreenPosition.z <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0);
    else if (vScreenPosition.z <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowMap1, lightSpacePos1, texelSize1 * 0.3);
    else if (vScreenPosition.z <= PssmSplitPoints.z)
        return CalcDepthShadow(shadowMap2, lightSpacePos2, texelSize2 * 0.2);
    else if (vScreenPosition.z <= PssmSplitPoints.w)
        return CalcDepthShadow(shadowMap3, lightSpacePos3, texelSize3 * 0.1);
    else
        return 0.0;
}
#endif


//----------------------------------------------------------------------------------------------------------------------
mediump float CalcDPSMShadow(const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, \
                             const sampler2D shadowMap0, const sampler2D shadowMap1, \
                             const mediump vec2 texelSize0, const mediump vec2 texelSize1)
{
    return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0);
}


#if MAX_SHADOW_TEXTURES > 0
//----------------------------------------------------------------------------------------------------------------------
mediump float GetShadow(const mediump int light, mediump int tex)
{
    mediump float shadow = 1.0;

    if (LightAttenuationArray[light].x > HALF_MAX_MINUS1) {
#if PSSM_SPLIT_COUNT == 4
#if MAX_SHADOW_TEXTURES > 2
        if (light == 0)
            shadow = CalcPSSMShadow(vLightSpacePosArray[0], vLightSpacePosArray[1], vLightSpacePosArray[2], vLightSpacePosArray[3], \
                                    ShadowMap0, ShadowMap1, ShadowMap2, ShadowMap3, \
                                    ShadowTexel0, ShadowTexel1, ShadowTexel2, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (light == 1)
            shadow = CalcPSSMShadow(vLightSpacePosArray[4], vLightSpacePosArray[5], vLightSpacePosArray[6], vLightSpacePosArray[7], \
                                    ShadowMap4, ShadowMap5, ShadowMap6, ShadowMap7, \
                                    ShadowTexel4, ShadowTexel5, ShadowTexel6, ShadowTexel7);
#endif
#endif

#if PSSM_SPLIT_COUNT == 3
#if MAX_SHADOW_TEXTURES > 2
        if (light == 0)
            shadow = CalcPSSMShadow(vLightSpacePosArray[0], vLightSpacePosArray[1], vLightSpacePosArray[2], \
                                    ShadowMap0, ShadowMap1, ShadowMap2, \
                                    ShadowTexel0, ShadowTexel1, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (light == 1)
            shadow = CalcPSSMShadow(vLightSpacePosArray[3], vLightSpacePosArray[4], vLightSpacePosArray[5], \
                                    ShadowMap3, ShadowMap4, ShadowMap5, \
                                    ShadowTexel3, ShadowTexel4, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 8
        else if (light == 2)
            shadow = CalcPSSMShadow(vLightSpacePosArray[6], vLightSpacePosArray[7], vLightSpacePosArray[8], \
                                    ShadowMap6, ShadowMap7, ShadowMap8, \
                                    ShadowTexel6, ShadowTexel7, ShadowTexel8);
#endif
#endif

#if PSSM_SPLIT_COUNT == 2
#if MAX_SHADOW_TEXTURES > 1
        if (tex == 0)
            shadow = CalcPSSMShadow(vLightSpacePosArray[0], vLightSpacePosArray[1], ShadowMap0, ShadowMap1, ShadowTexel0, ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 1)
            shadow = CalcPSSMShadow(vLightSpacePosArray[1], vLightSpacePosArray[2], ShadowMap1, ShadowMap2, ShadowTexel1, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 2)
            shadow = CalcPSSMShadow(vLightSpacePosArray[2], vLightSpacePosArray[3], ShadowMap2, ShadowMap3, ShadowTexel2, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 3)
            shadow = CalcPSSMShadow(vLightSpacePosArray[3], vLightSpacePosArray[4], ShadowMap3, ShadowMap4, ShadowTexel3, ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 4)
            shadow = CalcPSSMShadow(vLightSpacePosArray[4], vLightSpacePosArray[5], ShadowMap4, ShadowMap5, ShadowTexel4, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 5)
            shadow = CalcPSSMShadow(vLightSpacePosArray[5], vLightSpacePosArray[6], ShadowMap5, ShadowMap6, ShadowTexel5, ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 6)
            shadow = CalcPSSMShadow(vLightSpacePosArray[6], vLightSpacePosArray[7], ShadowMap6, ShadowMap7, ShadowTexel6, ShadowTexel7);
#endif
#if MAX_SHADOW_TEXTURES > 8
        else if (tex == 7)
            shadow = CalcPSSMShadow(vLightSpacePosArray[7], vLightSpacePosArray[8], ShadowMap7, ShadowMap8, ShadowTexel6, ShadowTexel7);
#endif
#endif

#if PSSM_SPLIT_COUNT == 1
#if MAX_SHADOW_TEXTURES > 0
        if (tex == 0)
            shadow = CalcPSSMShadow(ShadowMap0, vLightSpacePosArray[0], ShadowTexel0);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (tex == 1)
            shadow = CalcPSSMShadow(ShadowMap1, vLightSpacePosArray[1], ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 2)
            shadow = CalcPSSMShadow(ShadowMap2, vLightSpacePosArray[2], ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 3)
            shadow = CalcPSSMShadow(ShadowMap3, vLightSpacePosArray[3], ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 4)
            shadow = CalcPSSMShadow(ShadowMap4, vLightSpacePosArray[4], ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 5)
            shadow = CalcPSSMShadow(ShadowMap5, vLightSpacePosArray[5], ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 6)
            shadow = CalcPSSMShadow(ShadowMap6, vLightSpacePosArray[6], ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 7)
            shadow = CalcPSSMShadow(ShadowMap7, vLightSpacePosArray[7], ShadowTexel7);
#endif
#if MAX_SHADOW_TEXTURES > 8
        else if (tex == 8)
            shadow = CalcPSSMShadow(ShadowMap8, vLightSpacePosArray[8], ShadowTexel8);
#endif
#endif

        tex += PSSM_SPLIT_COUNT;

    }
    else if (LightSpotParamsArray[light].z > 0.0) {
#if MAX_SHADOW_TEXTURES > 1
        if (tex == 0)
            shadow = CalcDPSMShadow(vLightSpacePosArray[0], vLightSpacePosArray[1], ShadowMap0, ShadowMap1, ShadowTexel0, ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 1)
            shadow = CalcDPSMShadow(vLightSpacePosArray[1], vLightSpacePosArray[2], ShadowMap1, ShadowMap2, ShadowTexel1, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 2)
            shadow = CalcDPSMShadow(vLightSpacePosArray[2], vLightSpacePosArray[3], ShadowMap2, ShadowMap3, ShadowTexel2, ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 3)
            shadow = CalcDPSMShadow(vLightSpacePosArray[3], vLightSpacePosArray[4], ShadowMap3, ShadowMap4, ShadowTexel3, ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 4)
            shadow = CalcDPSMShadow(vLightSpacePosArray[4], vLightSpacePosArray[5], ShadowMap4, ShadowMap5, ShadowTexel4, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 5)
            shadow = CalcDPSMShadow(vLightSpacePosArray[5], vLightSpacePosArray[6], ShadowMap5, ShadowMap6, ShadowTexel5, ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 6)
            shadow = CalcDPSMShadow(vLightSpacePosArray[6], vLightSpacePosArray[7], ShadowMap6, ShadowMap7, ShadowTexel6, ShadowTexel7);
#endif
#if MAX_SHADOW_TEXTURES > 8
        else if (tex == 7)
            shadow = CalcDPSMShadow(vLightSpacePosArray[7], vLightSpacePosArray[8], ShadowMap7, ShadowMap8, ShadowTexel6, ShadowTexel7);
#endif

        tex += 2;

    } else {
#if MAX_SHADOW_TEXTURES > 0
        if (tex == 0)
            shadow = CalcDepthShadow(ShadowMap0, vLightSpacePosArray[0], ShadowTexel0);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (tex == 1)
            shadow = CalcDepthShadow(ShadowMap1, vLightSpacePosArray[1], ShadowTexel1);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 2)
            shadow = CalcDepthShadow(ShadowMap2, vLightSpacePosArray[2], ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 3)
            shadow = CalcDepthShadow(ShadowMap3, vLightSpacePosArray[3], ShadowTexel3);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 4)
            shadow = CalcDepthShadow(ShadowMap4, vLightSpacePosArray[4], ShadowTexel4);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 5)
            shadow = CalcDepthShadow(ShadowMap5, vLightSpacePosArray[5], ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 6)
            shadow = CalcDepthShadow(ShadowMap6, vLightSpacePosArray[6], ShadowTexel6);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 7)
            shadow = CalcDepthShadow(ShadowMap7, vLightSpacePosArray[7], ShadowTexel7);
#endif
#if MAX_SHADOW_TEXTURES > 8
        else if (tex == 8)
            shadow = CalcDepthShadow(ShadowMap8, vLightSpacePosArray[8], ShadowTexel8);
#endif

        tex += 1;

    }

    return shadow;
}


#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
