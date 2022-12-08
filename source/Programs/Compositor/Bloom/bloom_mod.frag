// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"


in vec2 vUV0;
uniform sampler2D uRT;
uniform sampler2D uRT0;
//uniform sampler2D uStarburst;
uniform vec2 TexelSize1;
uniform float mipCount;


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale3x3(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2(0.0, -1.0)).rgb;
  vec3 C = texture2D(tex, uv + tsize * vec2(1.0, -1.0)).rgb;
  vec3 D = texture2D(tex, uv + tsize * vec2(-1.0, 0.0)).rgb;
  vec3 E = texture2D(tex, uv + tsize                  ).rgb;
  vec3 F = texture2D(tex, uv + tsize * vec2(1.0, 0.0)).rgb;
  vec3 G = texture2D(tex, uv + tsize * vec2(-1.0, 1.0)).rgb;
  vec3 H = texture2D(tex, uv + tsize * vec2(0.0,  1.0)).rgb;
  vec3 I = texture2D(tex, uv + tsize * vec2(1.0, 1.0)).rgb;

  vec3 color = E * 0.25;
  color += (B + D + F + H) * 0.125;
  color += (A + C + G + I) * 0.0625;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
float SampleStarburst(const sampler2D tex, const vec2 _uv, const float offset)
{
  vec2 uv = _uv - vec2(0.5);
  float d = length(uv);
  float radial = acos(uv.x / d);
  float mask = texture2D(tex, vec2(radial + offset * 1.0, 0.0)).r * texture2D(tex, vec2(radial - offset * 0.5, 0.0)).r;
  mask = clamp(mask + (1.0 - smoothstep(0.0, 0.3, d)), 0.0, 1.0);

  return mask;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 rt = texture2D(uRT, vUV0).rgb;
  vec3 rt0 = Upscale3x3(uRT0, vUV0, TexelSize1);
  //float w = SampleStarburst(uStarburst, vUV0, 1.0);
  float w = 1.0 / mipCount;
#ifdef GL_ES
  w *= 2.0;
#endif

  FragColor.rgb = rt + rt0 * w;
}
