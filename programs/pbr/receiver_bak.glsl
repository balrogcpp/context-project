#ifndef RECEIVER_GLSL
#define RECEIVER_GLSL

#define SHADOWRECEIVER_PCF
#undef SHADOWRECEIVER_VSM
#undef SHADOWRECEIVER_ESM

#ifdef SHADOWRECEIVER_VSM
float VSM(vec2 moments, float compare){
  float m1 = moments.x;
  float diff = compare - m1;

  if(diff > 0.0)
  {
    float m2 = moments.y;
    float sigma2 = m2 - (m1 * m1);
    const float offset = -0.5;
    const float scale = 0.5;
    float shadow = offset + scale * sigma2 / (sigma2 + diff * diff);
    return clamp(shadow, 0, 1);
  }
  else
  {
    return 1.0;
  }
}
#endif
#ifdef SHADOWRECEIVER_ESM
float ESM(float depth, float compare){
  const float k = 10.0;
  const float offset = -0.5;
  const float scale = 0.5;

  if(compare > depth)
  {
    float shadow = scale * exp(k * (depth - compare)) + offset;
    return clamp(shadow, 0.0, 1.0);
  }
  else
  {
    return 1.0;
  }
}
#endif
#ifdef SHADOWRECEIVER_PCF
vec2 VogelDiskSample(int sampleIndex, int samplesCount, float phi)
{
  const float GoldenAngle = 2.4;

  float r = sqrt(sampleIndex + 0.5) / sqrt(samplesCount);
  float theta = sampleIndex * GoldenAngle + phi;
  float sine = sin(theta);
  float cosine = cos(theta);

  return vec2(r * cosine, r * sine);
}
//----------------------------------------------------------------------------------------------------------------------
float InterleavedGradientNoise(vec2 position_screen)
{
  vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
  return fract(magic.z * fract(dot(position_screen, magic.xy)));
}
//----------------------------------------------------------------------------------------------------------------------
float AvgBlockersDepthToPenumbra(float z_shadowMapView, float avgBlockersDepth)
{
  float penumbra = (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  penumbra *= penumbra;
  return clamp(80.0f * penumbra, 0.0, 1.0);
}
//----------------------------------------------------------------------------------------------------------------------
float Penumbra(sampler2D shadowMap, float gradientNoise, vec2 shadowMapUV, float z_shadowMapView, int samplesCount)
{
  float avgBlockersDepth = 0.0f;
  float blockersCount = 0.0f;
  const float penumbraFilterMaxSize = 0.001;

  for(int i = 0; i < samplesCount; i++)
  {
    vec2 sampleUV = VogelDiskSample(i, samplesCount, gradientNoise);
    sampleUV = shadowMapUV + penumbraFilterMaxSize * sampleUV;

    float sampleDepth = texture2D(shadowMap, sampleUV).x;

    if(sampleDepth < z_shadowMapView)
    {
      avgBlockersDepth += sampleDepth;
      blockersCount += 1.0f;
    }
  }

  if(blockersCount > 0.0f)
  {
    avgBlockersDepth /= blockersCount;
    return AvgBlockersDepthToPenumbra(z_shadowMapView, avgBlockersDepth);
  }
  else
  {
    return 0.0f;
  }
}
#endif
#ifndef SHADOWRECEIVER_PCF
// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i)
{
  vec4 seed4 = vec4(seed,i);
  float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
  return fract(sin(dot_product) * 43758.5453);
}
//----------------------------------------------------------------------------------------------------------------------
const vec2 poissonDisk16[16] = vec2[] (
vec2( -0.94201624, -0.39906216 ),
vec2( 0.94558609, -0.76890725 ),
vec2( -0.094184101, -0.92938870 ),
vec2( 0.34495938, 0.29387760 ),
vec2( -0.91588581, 0.45771432 ),
vec2( -0.81544232, -0.87912464 ),
vec2( -0.38277543, 0.27676845 ),
vec2( 0.97484398, 0.75648379 ),
vec2( 0.44323325, -0.97511554 ),
vec2( 0.53742981, -0.47373420 ),
vec2( -0.26496911, -0.41893023 ),
vec2( 0.79197514, 0.19090188 ),
vec2( -0.24188840, 0.99706507 ),
vec2( -0.81409955, 0.91437590 ),
vec2( 0.19984126, 0.78641367 ),
vec2( 0.14383161, -0.14100790 )
);
#endif

float calcDepthShadow(sampler2D shadowMap, vec4 uv)
{
  uv.xyz /= uv.w;

  uv.z = uv.z * 0.5 + 0.5; // convert -1..1 to 0..1
  float shadow = 0.0;
  float compare = uv.z;

#ifdef SHADOWRECEIVER_VSM
    const int iterations = 16;
    for (int i = 0; i < iterations; i++)
    {
      int index = int(16.0*random(vPosition, i))%16;
      shadow += (VSM(texture2D(shadowMap, uv.xy + poissonDisk16[index] * 0.001).rg, compare) / iterations);
    }
#endif
#ifdef SHADOWRECEIVER_ESM
    const int iterations = 16;
    for (int i = 0; i < iterations; i++)
    {
      int index = int(16.0*random(vPosition, i))%16;
      shadow += (ESM(texture2D(shadowMap, uv.xy + poissonDisk16[index] * 0.001).r, compare) / iterations);
    }
#endif
#ifdef SHADOWRECEIVER_PCF
  const int iterations = 16;
  for (int i = 0; i < iterations; i++)
  {
    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    //    float penumbra = Penumbra(shadowMap, gradientNoise, uv.xy, compare, 16);
    shadow += (texture2D(shadowMap, uv.xy + VogelDiskSample(i, 16, gradientNoise) * 0.002).r > compare ? 1.0 / float(iterations) : 0.0);
  }
#endif

  return clamp(shadow + uShadowColour, 0, 1);;
}
#endif //RECEIVER_GLSL