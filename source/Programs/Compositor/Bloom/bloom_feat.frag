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

  vec3 color =  vec3(
    texture2D(tex, uv + offset).r,
    texture2D(tex, uv).g,
    texture2D(tex, uv - offset).b
  );

  color /= (1.0 + luminance(LINEARtoSRGB(color)));
  return color;
}


// https://john-chapman.github.io/2017/11/05/pseudo-lens-flare.html
// Cubic window; map [0, _radius] in [1, 0] as a cubic falloff from _center.
float Window_Cubic(float _x, float _center, float _radius)
{
  _x = min(abs(_x - _center) / _radius, 1.0);
  return 1.0 - _x * _x * (3.0 - 2.0 * _x);
}


//----------------------------------------------------------------------------------------------------------------------
vec3 GhostFeatures(const sampler2D tex, const vec2 _uv, const vec2 texel, const int counter, const float chromaticRadius)
{
  vec2 uv = vec2(1.0) - _uv;
  vec2 ghostVec = (vec2(0.5) - uv) * 0.44;
  vec3 color = vec3(0.0);

  #define MAX_GHOST_COUNT 24

  // ghosts
  for (int i = 0; i < MAX_GHOST_COUNT; ++i) {
    if (counter <= i) break;

    vec2 suv = fract(uv + ghostVec * float(i));
    float d = distance(suv, vec2(0.5));
    float w = pow(1.0 - d, 5.0) / float(counter);
    vec3 s = SampleChromatic(tex, suv, chromaticRadius);
    color += s * w;
  }

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 HaloFeatures(const sampler2D tex, const vec2 _uv, const vec2 texel, const int counter, const float chromaticRadius)
{
  vec2 uv = vec2(1.0) - _uv;
  vec2 haloVec = vec2(0.5) - uv;

  // halo
  #define RADIUS 0.45
  float aspectRatio = texel.y / texel.x;
  haloVec.x *= aspectRatio;
  haloVec = normalize(haloVec);
  haloVec.x /= aspectRatio;
  vec2 wuv = (uv - vec2(0.5, 0.0)) / vec2(aspectRatio, 1.0) + vec2(0.5, 0.0);
  float d = distance(wuv, vec2(0.5));
  float w = pow(1.0 - d, 5.0);
  haloVec *= RADIUS;

  vec2 suv = uv + haloVec;
  vec3 s = SampleChromatic(tex, suv, chromaticRadius);
  vec3 color = s * w;

  return color;
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

  FragColor.rgb = color;
}
