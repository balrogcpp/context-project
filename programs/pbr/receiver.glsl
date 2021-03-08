//MIT License
//
//Copyright (c) 2021 Andrei Vasilev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

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
float CalcDepthShadow(sampler2D shadowMap, vec4 lightSpace, float offset, float filter_size, int iterations)
{
  lightSpace /= lightSpace.w;

  lightSpace.z = lightSpace.z * 0.5 + 0.5; // convert -1..1 to 0..1
  float shadow = 1.0;
  float compare = lightSpace.z;

  float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
//  float penumbra = Penumbra(shadowMap, gradientNoise, uv.xy, compare, 16);

  for (int i = 0; i < iterations; i++)
  {
    lightSpace.xy += VogelDiskSample(i, iterations, gradientNoise) * filter_size;
//    uv.xy += VogelDiskSample(i, iterations, gradientNoise) * penumbra * filter_size;
    float depth = texture2D(shadowMap, lightSpace.xy).r;
    shadow -= (( (depth - offset) < compare) ? 1.0 / float(iterations) : 0.0);
  }

  return shadow;
}

//----------------------------------------------------------------------------------------------------------------------
float CalcPSSMDepthShadow(vec4 pssmSplitPoints, vec4 lightSpacePos0, vec4 lightSpacePos1, vec4 lightSpacePos2, sampler2D shadowMap0, sampler2D shadowMap1, sampler2D shadowMap2, float camDepth, float offset, float filter_size, int iterations)
{
  // calculate shadow
  if (camDepth <= pssmSplitPoints.x)
  {
    return CalcDepthShadow(shadowMap0, lightSpacePos0, offset, filter_size/2.0, iterations);
  }
  else if (camDepth <= pssmSplitPoints.y)
  {
    return CalcDepthShadow(shadowMap1, lightSpacePos1, offset, filter_size/4.0, iterations);
  }
  else if (camDepth <= pssmSplitPoints.z)
  {
    return CalcDepthShadow(shadowMap2, lightSpacePos2, offset, filter_size/4.0, iterations);
  }

  return 1.0;
}

#endif //RECEIVER_GLSL
