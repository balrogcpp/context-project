// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "srgb.glsl"


in vec2 vUV0;
uniform sampler2D uSampler;
//uniform sampler2D uLensMask;
uniform vec2 TexelSize0;
uniform float uChromaticRadius;


//----------------------------------------------------------------------------------------------------------------------
vec3 SampleChromatic(const sampler2D tex, const vec2 _uv, const float radius)
{
  vec2 offset = normalize(vec2(0.5) - _uv) * radius;

  return vec3(
    texture2D(tex, _uv + offset).r,
    texture2D(tex, _uv).g,
    texture2D(tex, _uv - offset).b
  );
}

//----------------------------------------------------------------------------------------------------------------------
vec3 SampleGhosts(const sampler2D tex, const vec2 _uv)
{
  vec2 uv = vec2(1.0) - _uv;
  vec2 ghostVec = (vec2(0.5) - uv) * 0.5;
  vec3 ret = vec3(0.0);

  for (float i = 1.0; i < 2.0; i += 1.0)
  {
    vec2 suv = fract(uv + ghostVec * vec2(i));
    vec3 s = SampleChromatic(tex, suv, uChromaticRadius);

    float d = distance(suv, vec2(0.5));
    float weight = pow(1.0 - d, 10.0);

    ret += s * weight;
  }

  return ret;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 SampleHalo(const sampler2D tex, const vec2 _uv)
{
  vec2 uv = vec2(1.0) - _uv;
  vec2 haloVec = (vec2(0.5) - uv) * 0.5;
  float radius = 1.0;

  float aspectRatio = TexelSize0.x / TexelSize0.y;
  haloVec.x /= aspectRatio;
  haloVec = normalize(haloVec);
  haloVec.x *= aspectRatio;
  haloVec *= 0.5 * radius;

  float d = distance(fract(uv + haloVec), vec2(0.5));
  float weight = pow(1.0 - d, 10.0);
  vec3 s = SampleChromatic(tex, uv + haloVec, uChromaticRadius);

  return s * weight;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 SampleFeatures(const sampler2D tex, const vec2 _uv)
{
  vec2 uv = vec2(1.0) - _uv;
  vec2 ghostVec = (vec2(0.5) - uv) * 0.5;
  vec3 ret = vec3(0.0);

  // ghosts
  vec2 suv = fract(uv + ghostVec);
  float d = distance(suv, vec2(0.5));
  float w = pow(1.0 - d, 10.0);

  vec3 s0 = SampleChromatic(tex, suv, uChromaticRadius);
  ret += s0 * w;

  // halo
  const float radius = 1.0;
  float aspectRatio = TexelSize0.x / TexelSize0.y;
  ghostVec.x /= aspectRatio;
  ghostVec = normalize(ghostVec);
  ghostVec.x *= aspectRatio;
  ghostVec *= 0.5 * radius;

  vec3 s1 = SampleChromatic(tex, uv + ghostVec, uChromaticRadius);
  ret += s1 * w;

  return ret;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = Downscale4x4(uSampler, vUV0, TexelSize0);
  color += SampleFeatures(uSampler, vUV0);
  //color *= SRGBtoLINEAR(texture2D(uLensMask, vUV0).rgb);

  FragColor.rgb = color;
}
