// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL

// shadow receiver
#if MAX_SHADOW_TEXTURES > 0
varying highp vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
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
uniform mediump vec4 PssmSplitPoints;
uniform mediump float ShadowFilterSize;
uniform mediump int ShadowFilterIterations;
#endif // MAX_SHADOW_TEXTURES > 0

#include "math.glsl"
//#define PENUMBRA


//----------------------------------------------------------------------------------------------------------------------
mediump float InterleavedGradientNoise(const mediump vec2 position_screen)
{
    mediump vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec2 VogelDiskSample(const mediump float sampleIndex, const mediump float samplesCount, const mediump float phi)
{
    #define GOLDEN_ANGLE 2.4

    mediump float r = sqrt((sampleIndex + 0.5) / samplesCount);
    mediump float theta = sampleIndex * GOLDEN_ANGLE + phi;
    return vec2(r * cos(theta), r * sin(theta));
}


#ifdef PENUMBRA
//----------------------------------------------------------------------------------------------------------------------
mediump float AvgBlockersDepthToPenumbra(const mediump float z_shadowMapView, const mediump float avgBlockersDepth)
{
  mediump float penumbra = (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  return clamp(80.0 * penumbra * penumbra, 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
mediump float AvgBlockersDepthToPenumbra(const mediump float lightSize, const mediump float z_shadowMapView, const mediump float avgBlockersDepth)
{
  mediump float penumbra = lightSize * (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  return clamp(penumbra * penumbra, 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
mediump float Penumbra(const sampler2D shadowMap, const mediump vec2 shadowMapUV, const mediump float gradientNoise, const mediump float z_shadowMapView, const mediump float penumbraFilterMaxSize, const mediump int iterations)
{
  mediump float avgBlockersDepth = 0.0;
  mediump float blockersCount = HALF_EPSILON;

  #define MAX_SAMPLES 32

  for (int i = 0; i < MAX_SAMPLES; ++i) {
    if (iterations <= i) break;

    mediump vec2 sampleUV = VogelDiskSample(float(i), float(iterations), float(gradientNoise));
    sampleUV = shadowMapUV + penumbraFilterMaxSize * sampleUV;
    mediump float sampleDepth = texture2D(shadowMap, sampleUV).x;

    mediump float depthTest = clamp((z_shadowMapView - sampleDepth), 0.0, 1.0);
    avgBlockersDepth += depthTest * sampleDepth;
    blockersCount += depthTest;
  }

  return AvgBlockersDepthToPenumbra(10000.0, z_shadowMapView, avgBlockersDepth / blockersCount);
}

#endif // PENUMBRA


//----------------------------------------------------------------------------------------------------------------------
mediump float FetchTerraShadow2(const sampler2D shadowMap, mediump vec2 uv, const mediump vec2 tsize)
{
  mediump float shadow = 0.0;
  mediump vec2 filterSize = 2.0 * tsize;
  mediump float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);

  #define MAX_SAMPLES 4
  #define NUM_SAMPLES 4.0

  for (int i = 0; i < MAX_SAMPLES; ++i) {
    mediump vec2 offset = VogelDiskSample(float(i), NUM_SAMPLES, gradientNoise) * filterSize;

    uv += offset;
    shadow += texture2D(shadowMap, uv.xy).x;
  }

  shadow *= (1.0 / NUM_SAMPLES);

  return shadow;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float FetchTerraShadow(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).r;
    mediump float B = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).r;
    mediump float C = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).r;
    mediump float D = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).r;

    mediump float c1 = (A + B + C + D) * 0.25;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float CalcDepthShadow(const sampler2D shadowMap, mediump vec4 lightSpace, const mediump vec2 filterSize, const mediump int iterations)
{
  lightSpace /= lightSpace.w;
  lightSpace.z = lightSpace.z * 0.5 + 0.5;

  mediump float shadow = 0.0;
  mediump float currentDepth = lightSpace.z + HALF_EPSILON * (1.0 - F0);
  mediump float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
#ifdef PENUMBRA
  #define MAX_PENUMBRA_FILTER 0.05
  mediump float penumbra = Penumbra(shadowMap, lightSpace.xy, gradientNoise, currentDepth, MAX_PENUMBRA_FILTER, iterations);
#endif

  #define MAX_SAMPLES 32

  for (int i = 0; i < MAX_SAMPLES; ++i) {
    if (iterations <= i) break;

    mediump vec2 offset = VogelDiskSample(float(i), float(iterations), gradientNoise) * filterSize;
#ifdef PENUMBRA
    offset *= penumbra;
#endif

    lightSpace.xy += offset;
    mediump float depth = texture2D(shadowMap, lightSpace.xy).x;
    shadow += bigger(depth, currentDepth);
  }

  shadow *= (1.0 / iterations);

  return shadow;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float CalcPSSMShadow (const mediump vec4 PssmSplitPoints, \
                                    const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, const mediump vec4 lightSpacePos2, \
                                    const sampler2D shadowMap0, const sampler2D shadowMap1, const sampler2D shadowMap2, \
                                    const mediump vec2 texelSize0, const mediump vec2 texelSize1, const mediump vec2 texelSize2)
{
    // calculate shadow
    if (vDepth <= PssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0 * ShadowFilterSize, ShadowFilterIterations);
    else if (vDepth <= PssmSplitPoints.y)
        return CalcDepthShadow(shadowMap1, lightSpacePos1, texelSize1 * ShadowFilterSize, ShadowFilterIterations);
    else if (vDepth <= PssmSplitPoints.z)
        return CalcDepthShadow(shadowMap2, lightSpacePos2, texelSize2 * ShadowFilterSize, ShadowFilterIterations);
    else
        return 1.0;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float CalcDPSMShadow(const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, \
                                    const sampler2D shadowMap0, const sampler2D shadowMap1, \
                                    const mediump vec2 texelSize0, const mediump vec2 texelSize1)
{
    return CalcDepthShadow(shadowMap0, lightSpacePos0, texelSize0 * ShadowFilterSize, ShadowFilterIterations);
}

#if MAX_SHADOW_TEXTURES > 0
//----------------------------------------------------------------------------------------------------------------------
mediump float GetShadow(const mediump int light, mediump int tex)
{
    mediump float shadow = 1.0;

    if (LightAttenuationArray[light].x > HALF_MAX_MINUS1) {
#if MAX_SHADOW_TEXTURES > 2
        if (light == 0)
            shadow = CalcPSSMShadow(PssmSplitPoints, \
                                    vLightSpacePosArray[0], vLightSpacePosArray[1], vLightSpacePosArray[2], \
                                    ShadowMap0, ShadowMap1, ShadowMap2, \
                                    ShadowTexel0, ShadowTexel1, ShadowTexel2);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (light == 1)
            shadow = CalcPSSMShadow(PssmSplitPoints, \
                                    vLightSpacePosArray[3], vLightSpacePosArray[4], vLightSpacePosArray[5], \
                                    ShadowMap3, ShadowMap4, ShadowMap5, \
                                    ShadowTexel3, ShadowTexel4, ShadowTexel5);
#endif
#if MAX_SHADOW_TEXTURES > 8
        else if (light == 2)
            shadow = CalcPSSMShadow(PssmSplitPoints, \
                                    vLightSpacePosArray[6], vLightSpacePosArray[7], vLightSpacePosArray[8], \
                                    ShadowMap6, ShadowMap7, ShadowMap8, \
                                    ShadowTexel6, ShadowTexel7, ShadowTexel8);
#endif
        tex += 3;
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
            shadow = CalcDepthShadow(ShadowMap0, vLightSpacePosArray[0], ShadowTexel0 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 1
        else if (tex == 1)
            shadow = CalcDepthShadow(ShadowMap1, vLightSpacePosArray[1], ShadowTexel1 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 2
        else if (tex == 2)
            shadow = CalcDepthShadow(ShadowMap2, vLightSpacePosArray[2], ShadowTexel2 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 3
        else if (tex == 3)
            shadow = CalcDepthShadow(ShadowMap3, vLightSpacePosArray[3], ShadowTexel3 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (tex == 4)
            shadow = CalcDepthShadow(ShadowMap4, vLightSpacePosArray[4], ShadowTexel4 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (tex == 5)
            shadow = CalcDepthShadow(ShadowMap5, vLightSpacePosArray[5], ShadowTexel5 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 6
        else if (tex == 6)
            shadow = CalcDepthShadow(ShadowMap6, vLightSpacePosArray[6], ShadowTexel6 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 7
        else if (tex == 7)
            shadow = CalcDepthShadow(ShadowMap7, vLightSpacePosArray[7], ShadowTexel7 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 8
        else if (tex == 8)
            shadow = CalcDepthShadow(ShadowMap8, vLightSpacePosArray[8], ShadowTexel8 * ShadowFilterSize, ShadowFilterIterations);
#endif
        tex += 1;
    }

    return shadow;
}


#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
