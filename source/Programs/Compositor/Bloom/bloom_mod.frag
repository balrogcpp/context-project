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
uniform vec2 TexelSize1;
uniform float mipCount;



//----------------------------------------------------------------------------------------------------------------------
vec3 Upscale3x3(const sampler2D sampler, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(sampler, uv + tsize * vec2(-1.0, -1.0)).rgb;
  vec3 B = texture2D(sampler, uv + tsize * vec2(0.0, -1.0)).rgb;
  vec3 C = texture2D(sampler, uv + tsize * vec2(1.0, -1.0)).rgb;
  vec3 D = texture2D(sampler, uv + tsize * vec2(-1.0, 0.0)).rgb;
  vec3 E = texture2D(sampler, uv + tsize                  ).rgb;
  vec3 F = texture2D(sampler, uv + tsize * vec2(1.0, 0.0)).rgb;
  vec3 G = texture2D(sampler, uv + tsize * vec2(-1.0, 1.0)).rgb;
  vec3 H = texture2D(sampler, uv + tsize * vec2(0.0,  1.0)).rgb;
  vec3 I = texture2D(sampler, uv + tsize * vec2(1.0, 1.0)).rgb;

  vec3 color = E * 0.25;
  color += (B + D + F + H) * 0.125;
  color += (A + C + G + I) * 0.0625;

  return color;
}



//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 rt = texture2D(uRT, vUV0).rgb;
  vec3 rt0 = Upscale3x3(uRT0, vUV0, TexelSize1);

  FragColor.rgb = rt + rt0 * (1.0 / mipCount);
}
