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


in vec2 vUV0;
uniform sampler2D uSampler;
uniform sampler2D uDepth;
uniform float FarClipDistance;
uniform vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
vec3 Downscale13T(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).rgb;
  vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).rgb;
  vec3 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).rgb;
  vec3 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).rgb;
  vec3 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).rgb;
  vec3 G = texture2D(tex, uv                           ).rgb;
  vec3 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).rgb;
  vec3 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).rgb;
  vec3 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).rgb;
  vec3 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).rgb;
  vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).rgb;
  vec3 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).rgb;

  vec3 c1 = (D + E + I + J); c1 /= (1.0 + luminance(LINEARtoSRGB(c1))); c1 *= 0.125;
  vec3 c2 = (A + B + G + F); c2 /= (1.0 + luminance(LINEARtoSRGB(c2))); c2 *= 0.03125;
  vec3 c3 = (B + C + H + G); c3 /= (1.0 + luminance(LINEARtoSRGB(c3))); c3 *= 0.03125;
  vec3 c4 = (F + G + L + K); c4 /= (1.0 + luminance(LINEARtoSRGB(c4))); c4 *= 0.03125;
  vec3 c5 = (G + H + M + L); c5 /= (1.0 + luminance(LINEARtoSRGB(c5))); c5 *= 0.03125;

  return c1 + c2 + c3 + c4 + c5;
}



//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uSampler, vUV0).rgb;
  color /= (1.0 + luminance((color)));
  float depth = texture2D(uDepth, vUV0).w;
  FragColor.rgb = bigger(depth, 0.5) * color;
}