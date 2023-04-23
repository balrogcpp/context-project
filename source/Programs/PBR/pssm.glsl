// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL


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
mediump float Penumbra(const mediump sampler2D shadowMap, const mediump vec2 shadowMapUV, const mediump float gradientNoise, const mediump float z_shadowMapView, const mediump float penumbraFilterMaxSize, const mediump int iterations)
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
mediump float CalcDepthShadow(const mediump sampler2D shadowMap, mediump vec4 lightSpace, const mediump vec2 filterSize, const mediump int iterations)
{
  lightSpace /= lightSpace.w;
  lightSpace.z = lightSpace.z * 0.5 + 0.5;

  mediump float shadow = 1.0;
  mediump float currentDepth = lightSpace.z + HALF_EPSILON * (1.0 - F0);

  #define MAX_PENUMBRA_FILTER 0.05

  mediump float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
#ifdef PENUMBRA
  mediump float penumbra = Penumbra(shadowMap, lightSpace.xy, gradientNoise, currentDepth, MAX_PENUMBRA_FILTER, iterations);
#endif

  #define MAX_SAMPLES 32

  for (int i = 0; i < MAX_SAMPLES; ++i) {
    if (iterations <= i) break;

    mediump vec2 offset = VogelDiskSample(float(i), float(iterations), float(gradientNoise)) * filterSize;
#ifdef PENUMBRA
    offset *= penumbra;
#endif

    lightSpace.xy += offset;
    mediump float depth = texture2D(shadowMap, lightSpace.xy).x;
    shadow -= bigger(currentDepth, depth) * (1.0 / float(iterations));
  }

  return shadow;
}



#if MAX_SHADOW_TEXTURES > 0
//----------------------------------------------------------------------------------------------------------------------
mediump sampler2D GetShadowMap(const mediump int counter)
{
#if MAX_SHADOW_TEXTURES > 0
    if (counter == 0)
        return ShadowMap0;
#endif
#if MAX_SHADOW_TEXTURES > 1
    if (counter == 1)
        return ShadowMap1;
#endif
#if MAX_SHADOW_TEXTURES > 2
    if (counter == 2)
        return ShadowMap2;
#endif
#if MAX_SHADOW_TEXTURES > 3
    if (counter == 3)
        return ShadowMap3;
#endif
#if MAX_SHADOW_TEXTURES > 4
    if (counter == 4)
        return ShadowMap4;
#endif
#if MAX_SHADOW_TEXTURES > 5
    if (counter == 5)
        return ShadowMap5;
#endif
#if MAX_SHADOW_TEXTURES > 6
    if (counter == 6)
        return ShadowMap6;
#endif
#if MAX_SHADOW_TEXTURES > 7
    if (counter == 7)
        return ShadowMap7;
#endif

    return ShadowMap0;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec2 GetShadowTexel(const mediump int counter)
{
#if MAX_SHADOW_TEXTURES > 0
    if (counter == 0)
        return ShadowTexel0;
#endif
#if MAX_SHADOW_TEXTURES > 1
    if (counter == 1)
        return ShadowTexel1;
#endif
#if MAX_SHADOW_TEXTURES > 2
    if (counter == 2)
        return ShadowTexel2;
#endif
#if MAX_SHADOW_TEXTURES > 3
    if (counter == 3)
        return ShadowTexel3;
#endif
#if MAX_SHADOW_TEXTURES > 4
    if (counter == 4)
        return ShadowTexel4;
#endif
#if MAX_SHADOW_TEXTURES > 5
    if (counter == 5)
        return ShadowTexel5;
#endif
#if MAX_SHADOW_TEXTURES > 6
    if (counter == 6)
        return ShadowTexel6;
#endif
#if MAX_SHADOW_TEXTURES > 7
    if (counter == 7)
        return ShadowTexel7;
#endif

    return ShadowTexel0;
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


//----------------------------------------------------------------------------------------------------------------------
mediump float GetShadow(const mediump int light, mediump int tex)
{
    mediump float shadow = 1.0;

    if (LightAttenuationArray[light].x > HALF_MAX_MINUS1) {
        shadow = CalcPSSMShadow(PssmSplitPoints, \
                                vLightSpacePosArray[tex], vLightSpacePosArray[tex + 1], vLightSpacePosArray[tex + 2], \
                                GetShadowMap(tex), GetShadowMap(tex + 1), GetShadowMap(tex + 2), \
                                GetShadowTexel(tex), GetShadowTexel(tex + 1), GetShadowTexel(tex + 2));
        tex += 3;
    }
    else if (LightSpotParamsArray[light].z > 0.0) {
        shadow = CalcDPSMShadow(vLightSpacePosArray[tex], vLightSpacePosArray[tex + 1], GetShadowMap(tex), GetShadowMap(tex + 1), GetShadowTexel(tex), GetShadowTexel(tex + 1));
        tex += 2;
    } else {
        shadow = CalcDepthShadow(GetShadowMap(tex), vLightSpacePosArray[tex], GetShadowTexel(tex) * ShadowFilterSize, ShadowFilterIterations);
        tex += 1;
    }

    return shadow;
}

#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
