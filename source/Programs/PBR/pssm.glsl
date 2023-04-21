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
mediump float CalcDepthShadow(const mediump sampler2D shadowMap, mediump vec4 lightSpace, const mediump vec2 filter_size, const mediump int iterations)
{
  lightSpace /= lightSpace.w;
  lightSpace.z = lightSpace.z * 0.5 + 0.5;

  mediump float shadow = 1.0;
  mediump float current_depth = lightSpace.z + HALF_EPSILON * (1.0 - F0);

  #define MAX_PENUMBRA_FILTER 0.05

  mediump float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
#ifdef PENUMBRA
  mediump float penumbra = Penumbra(shadowMap, lightSpace.xy, gradientNoise, current_depth, MAX_PENUMBRA_FILTER, iterations);
#endif

  #define MAX_SAMPLES 32

  for (int i = 0; i < MAX_SAMPLES; ++i) {
    if (iterations <= i) break;

    mediump vec2 offset = VogelDiskSample(float(i), float(iterations), float(gradientNoise)) * filter_size;
#ifdef PENUMBRA
    offset *= penumbra;
#endif

    lightSpace.xy += offset;
    mediump float depth = texture2D(shadowMap, lightSpace.xy).x;
    shadow -= bigger(current_depth, depth) * (1.0 / float(iterations));
  }

  return shadow;
}



#if MAX_SHADOW_TEXTURES > 0
//----------------------------------------------------------------------------------------------------------------------
mediump float GetShadow(const mediump int counter)
{
    if (vDepth >= PssmSplitPoints.w)
        return 1.0;
#if MAX_SHADOW_TEXTURES > 0
    else if (counter == 0)
        return CalcDepthShadow(ShadowMap0, vLightSpacePosArray[0], ShadowTexel0 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 1
    else if (counter == 1)
        return CalcDepthShadow(ShadowMap1, vLightSpacePosArray[1], ShadowTexel1 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 2
    else if (counter == 2)
        return CalcDepthShadow(ShadowMap2, vLightSpacePosArray[2], ShadowTexel2 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 3
    else if (counter == 3)
        return CalcDepthShadow(ShadowMap3, vLightSpacePosArray[3], ShadowTexel3 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 4
    else if (counter == 4)
        return CalcDepthShadow(ShadowMap4, vLightSpacePosArray[4], ShadowTexel4 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 5
    else if (counter == 5)
        return CalcDepthShadow(ShadowMap5, vLightSpacePosArray[5], ShadowTexel5 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 6
    else if (counter == 6)
        return CalcDepthShadow(ShadowMap6, vLightSpacePosArray[6], ShadowTexel6 * ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 7
    else if (counter == 7)
        return CalcDepthShadow(ShadowMap7, vLightSpacePosArray[7], ShadowTexel7 * ShadowFilterSize, ShadowFilterIterations);
#endif

    return 1.0;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float CalcPSSMDepthShadow(const mediump vec4 PssmSplitPoints, const mediump vec4 lightSpacePos0, const mediump vec4 lightSpacePos1, const mediump vec4 lightSpacePos2, const sampler2D ShadowMap0, const sampler2D ShadowMap1, const sampler2D ShadowMap2)
{
    // calculate shadow
    if (vDepth <= PssmSplitPoints.x)
        return CalcDepthShadow(ShadowMap0, lightSpacePos0, ShadowTexel0 * ShadowFilterSize, ShadowFilterIterations);
    else if (vDepth <= PssmSplitPoints.y)
        return CalcDepthShadow(ShadowMap1, lightSpacePos1, ShadowTexel1 * ShadowFilterSize, ShadowFilterIterations);
    else if (vDepth <= PssmSplitPoints.z)
        return CalcDepthShadow(ShadowMap2, lightSpacePos2, ShadowTexel2 * ShadowFilterSize, ShadowFilterIterations);
    else
        return 1.0;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float GetPSSMShadow(const mediump int number)
{
    //  special case, as only light #0 can be with pssm shadows
    if (LightAttenuationArray[0].x > 10000.0)
    {
        if (number == 0)
            return CalcPSSMDepthShadow(PssmSplitPoints, \
                                    vLightSpacePosArray[0], vLightSpacePosArray[1], vLightSpacePosArray[2], \
                                    ShadowMap0, ShadowMap1, ShadowMap2);
        else
            return GetShadow(number + 2);
    } else {
        return GetShadow(number);
    }
}
#endif // MAX_SHADOW_TEXTURES > 0


#endif // PSSM_GLSL
