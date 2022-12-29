// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "math.glsl"
#include "srgb.glsl"


//----------------------------------------------------------------------------------------------------------------------
vec3 SampleChromatic(const sampler2D tex, const vec2 uv, const float radius)
{
  vec2 offset = normalize(vec2(0.5) - uv) * radius;

  return vec3(
    texture2D(tex, uv + offset).r,
    texture2D(tex, uv).g,
    texture2D(tex, uv - offset).b
  );
}


//----------------------------------------------------------------------------------------------------------------------
vec3 GhostFeatures(const sampler2D tex, const vec2 _uv, const vec2 texel, const int counter, const float chromaticRadius)
{
  vec2 uv = vec2(1.0) - _uv;
  vec2 ghostVec = (vec2(0.5) - uv) * 0.5;
  vec3 ret = vec3(0.0);

  #define MAX_GHOST_COUNT 12

  // ghosts
  for (int i = 0; i < MAX_GHOST_COUNT; ++i) {
    if (counter <= i) break;

    vec2 suv = fract(uv + ghostVec * float(i));
    float d = distance(suv, vec2(0.5));
    float w = pow(1.0 - d, 10.0) * (1.0 / float(counter)) * 0.5;
    vec3 s0 = SampleChromatic(tex, suv, chromaticRadius);
    ret += s0 * w;
  }

  return ret;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 HaloFeatures(const sampler2D tex, const vec2 _uv, const vec2 texel, const int counter, const float chromaticRadius)
{
  vec2 uv = vec2(1.0) - _uv;
  vec2 ghostVec = (vec2(0.5) - uv) * 0.5;
  vec3 ret = vec3(0.0);

  #define MAX_GHOST_COUNT 12

  // halo
  #define RADIUS 1.0
  float aspectRatio = texel.x / texel.y;
  ghostVec.x /= aspectRatio;
  ghostVec = normalize(ghostVec);
  ghostVec.x *= aspectRatio;
  ghostVec *= 0.5 * RADIUS;

  vec2 suv = fract(uv + ghostVec);
  float d = distance(suv, vec2(0.5));
  float w = pow(1.0 - d, 10.0) * 0.5;
  vec3 s1 = SampleChromatic(tex, suv, chromaticRadius);
  ret += s1 * w;

  return ret;
}


in vec2 vUV0;

uniform sampler2D uSampler;
uniform vec2 TexelSize0;
uniform float uChromaticRadius;
uniform int uFeaturesCount;



//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uSampler, vUV0).rgb;
  color += GhostFeatures(uSampler, vUV0, TexelSize0, uFeaturesCount, uChromaticRadius);
  color += HaloFeatures(uSampler, vUV0, TexelSize0, uFeaturesCount, uChromaticRadius);

  FragColor.rgb = SafeHDR(color);
}
