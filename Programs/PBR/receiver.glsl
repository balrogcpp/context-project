// This source file is part of "glue project". Created by Andrew Vasiliev

#ifndef RECEIVER_GLSL
#define RECEIVER_GLSL

//----------------------------------------------------------------------------------------------------------------------
float InterleavedGradientNoise(vec2 position_screen)
{
  vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
  return fract(magic.z * fract(dot(position_screen, magic.xy)));
}

//----------------------------------------------------------------------------------------------------------------------
vec2 VogelDiskSample(int sampleIndex, int samplesCount, float phi)
{
  const float GoldenAngle = 2.4;

  float r = sqrt(float(sampleIndex) + 0.5) / sqrt(float(samplesCount));
  float theta = float(sampleIndex) * GoldenAngle + phi;
  float sine = sin(theta);
  float cosine = cos(theta);

  return vec2(r * cosine, r * sine);
}

//----------------------------------------------------------------------------------------------------------------------
float AvgBlockersDepthToPenumbra(float z_shadowMapView, float avgBlockersDepth)
{
  float penumbra = (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  penumbra *= penumbra;
  return clamp(80.0 * penumbra, 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
float AvgBlockersDepthToPenumbra(float lightSize, float z_shadowMapView, float avgBlockersDepth)
{
  float penumbra = lightSize * (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  penumbra *= penumbra;
  return clamp(penumbra, 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
float Penumbra(sampler2D shadowMap, float gradientNoise, vec2 shadowMapUV, float z_shadowMapView, int samplesCount)
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

//----------------------------------------------------------------------------------------------------------------------
float CalcDepthShadow(sampler2D shadowMap, vec4 lightSpace, const float offset, const float filter_size, const int iterations)
{
  lightSpace /= lightSpace.w;

  lightSpace.z = lightSpace.z * 0.5 + 0.5; // convert -1..1 to 0..1
  float shadow = 1.0;
  float current_depth = lightSpace.z;

  float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
  //float penumbra = Penumbra(shadowMap, gradientNoise, lightSpace.xy, current_depth, iterations);

//  float depth = texture2D(shadowMap, lightSpace.xy).x - offset;
//  return (depth > current_depth) ? 1.0 : 0.0;

  for (int i = 0; i < iterations; i++)
  {
    lightSpace.xy += VogelDiskSample(i, iterations, gradientNoise) * filter_size;
    //lightSpace.xy += VogelDiskSample(i, iterations, gradientNoise) * penumbra * filter_size;
    float depth = texture2D(shadowMap, lightSpace.xy).x - offset;
    shadow -= ((depth < current_depth) ? 1.0 / float(iterations) : 0.0);
  }

  return shadow;


//  vec2 moments = texture2D(shadowMap, lightSpace.xy).xy;
//  float variance = moments.y - (moments.x * moments.x);
//  variance = max(variance, 0.00002);
//
//  float d = current_depth - moments.x;
//  float pMax = variance / (variance + d * d);
//  return smoothstep(0.1f, 1.0f, pMax);

}


#endif //RECEIVER_GLSL
