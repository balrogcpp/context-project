// created by Andrey Vasiliev

#ifndef RECEIVER_GLSL
#define RECEIVER_GLSL


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


#endif //RECEIVER_GLSL
