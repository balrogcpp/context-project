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
float calcDepthShadow(sampler2D shadowMap, vec4 uv)
{
  uv.xyz /= uv.w;

  uv.z = uv.z * 0.5 + 0.5; // convert -1..1 to 0..1
  float shadow = 0.0;
  float compare = uv.z;

  const int iterations = 16;
  const float shadowFilterMaxSize = 0.005;
  float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
//  float penumbra = Penumbra(shadowMap, gradientNoise, uv.xy, compare, 16);

  for (int i = 0; i < iterations; i++)
  {
    uv.xy += VogelDiskSample(i, 16, gradientNoise) * shadowFilterMaxSize;
//    uv.xy += VogelDiskSample(i, 16, gradientNoise) * penumbra * shadowFilterMaxSize;
    shadow += (texture2D(shadowMap, uv.xy).r > compare ? 1.0 / float(iterations) : 0.0);
  }

  return shadow;
}
#endif //RECEIVER_GLSL