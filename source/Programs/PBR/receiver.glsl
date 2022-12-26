// created by Andrey Vasiliev

#ifndef RECEIVER_GLSL
#define RECEIVER_GLSL


#include "math.glsl"
//#define PENUMBRA


//----------------------------------------------------------------------------------------------------------------------
float InterleavedGradientNoise(const vec2 position_screen)
{
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}


//----------------------------------------------------------------------------------------------------------------------
vec2 VogelDiskSample(const float sampleIndex, const float samplesCount, const float phi)
{
    #define GOLDEN_ANGLE 2.4

    float r = sqrt((sampleIndex + 0.5) / samplesCount);
    float theta = sampleIndex * GOLDEN_ANGLE + phi;

    return vec2(r * cos(theta), r * sin(theta));
}


#ifdef PENUMBRA
//----------------------------------------------------------------------------------------------------------------------
float AvgBlockersDepthToPenumbra(const float z_shadowMapView, const float avgBlockersDepth)
{
  float penumbra = (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  return clamp(80.0 * penumbra * penumbra, 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
float AvgBlockersDepthToPenumbra(const float lightSize, const float z_shadowMapView, const float avgBlockersDepth)
{
  float penumbra = lightSize * (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  return clamp(penumbra * penumbra, 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
float Penumbra(const sampler2D shadowMap, const vec2 shadowMapUV, const float gradientNoise, const float z_shadowMapView, const float penumbraFilterMaxSize, const int iterations)
{
  float avgBlockersDepth = 0.0;
  float blockersCount = HALF_EPSILON;
  #define MAX_SAMPLES 32

  for (int i = 0; i < MAX_SAMPLES; ++i) {
    if (iterations <= i) break;

    vec2 sampleUV = VogelDiskSample(float(i), float(iterations), float(gradientNoise));
    sampleUV = shadowMapUV + penumbraFilterMaxSize * sampleUV;
    float sampleDepth = texture2D(shadowMap, sampleUV).x;

    float depthTest = clamp((z_shadowMapView - sampleDepth), 0.0, 1.0);
    avgBlockersDepth += depthTest * sampleDepth;
    blockersCount += depthTest;
  }


  return AvgBlockersDepthToPenumbra(10000.0, z_shadowMapView, avgBlockersDepth / blockersCount);
}

#endif // PENUMBRA


//----------------------------------------------------------------------------------------------------------------------
float CalcDepthShadow(const sampler2D shadowMap, vec4 lightSpace, const vec2 filter_size, const int iterations)
{
  lightSpace /= lightSpace.w;

  lightSpace.z = lightSpace.z * 0.5 + 0.5; // convert -1..1 to 0..1
  float shadow = 1.0;
  float current_depth = lightSpace.z;
  #define MAX_PENUMBRA_FILTER 0.05

  float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
#ifdef PENUMBRA
  float penumbra = Penumbra(shadowMap, lightSpace.xy, gradientNoise, current_depth, MAX_PENUMBRA_FILTER, iterations);
#endif

  #define MAX_SAMPLES 32
  for (int i = 0; i < MAX_SAMPLES; ++i) {
    if (iterations <= i) break;

    vec2 offset = VogelDiskSample(float(i), float(iterations), float(gradientNoise)) * filter_size;
#ifdef PENUMBRA
    offset *= penumbra;
#endif
    lightSpace.xy += offset;
    float depth = texture2D(shadowMap, lightSpace.xy).x;
    shadow -= bigger(current_depth, depth) * (1.0 / float(iterations));
  }

  return shadow;
}


#endif //RECEIVER_GLSL
