/// created by Andrey Vasiliev

#ifndef RECEIVER_GLSL
#define RECEIVER_GLSL

#include "noise.glsl"

#ifdef PENUMBRA

//----------------------------------------------------------------------------------------------------------------------
float AvgBlockersDepthToPenumbra(const float z_shadowMapView, const float avgBlockersDepth)
{
  float penumbra = (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  penumbra *= penumbra;
  return clamp(80.0 * penumbra, 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
float AvgBlockersDepthToPenumbra(const float lightSize, const float z_shadowMapView, const float avgBlockersDepth)
{
  float penumbra = lightSize * (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  penumbra *= penumbra;
  return clamp(penumbra, 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
float Penumbra(const sampler2D shadowMap, const vec2 shadowMapUV, const float gradientNoise, const float z_shadowMapView, const int samplesCount)
{
  float avgBlockersDepth = 0.0;
  float blockersCount = 0.0;
  const float penumbraFilterMaxSize = 0.01;

  for(int i = 0; i < samplesCount; i++)
  {
    vec2 sampleUV = VogelDiskSample(i, samplesCount, gradientNoise);
    sampleUV = shadowMapUV + penumbraFilterMaxSize * sampleUV;

    float sampleDepth = texture2D(shadowMap, sampleUV).x;

    if(sampleDepth < z_shadowMapView)
    {
      avgBlockersDepth += sampleDepth;
      blockersCount += 1.0;
    }
  }

  if(blockersCount > 0.0)
  {
    avgBlockersDepth /= blockersCount;
    return AvgBlockersDepthToPenumbra(10000.0, z_shadowMapView, avgBlockersDepth);
  }
  else
  {
    return 0.0;
  }
}

#endif // PENUMBRA

//----------------------------------------------------------------------------------------------------------------------
float CalcDepthShadow(const sampler2D shadowMap, vec4 lightSpace, const float offset, const float filter_size, const int iterations)
{
  lightSpace /= lightSpace.w;

  lightSpace.z = lightSpace.z * 0.5 + 0.5; // convert -1..1 to 0..1
  float shadow = 1.0;
  float current_depth = lightSpace.z;

  float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
  //float penumbra = Penumbra(shadowMap, gradientNoise, lightSpace.xy, current_depth, iterations);

  for (int i = 0; i < iterations; i++)
  {
    lightSpace.xy += VogelDiskSample(i, iterations, gradientNoise) * filter_size;
    //lightSpace.xy += VogelDiskSample(i, iterations, gradientNoise) * penumbra * filter_size;
    float depth = texture2D(shadowMap, lightSpace.xy).x - offset;
    shadow -= ((depth < current_depth) ? 1.0 / float(iterations) : 0.0);
  }

  return shadow;
}


#endif //RECEIVER_GLSL
