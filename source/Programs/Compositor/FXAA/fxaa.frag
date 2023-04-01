// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"


//----------------------------------------------------------------------------------------------------------------------
vec3 FXAA(const sampler2D tex, const vec2 uv, const vec2 tsize, const float strength)
{
  const float reducemul = 0.125; // 1/8
  const float reducemin = 0.0078125; // 1/128
  const vec3 gray = vec3(0.299, 0.587, 0.114);

  vec3 basecol = texture2D(tex, uv).rgb;
  vec3 baseNW = texture2D(tex, uv - tsize).rgb;
  vec3 baseNE = texture2D(tex, uv + vec2(tsize.x, -tsize.y)).rgb;
  vec3 baseSW = texture2D(tex, uv + vec2(-tsize.x, tsize.y)).rgb;
  vec3 baseSE = texture2D(tex, uv + tsize).rgb;
  float monocol = dot(basecol, gray);
  float monoNW = dot(baseNW, gray);
  float monoNE = dot(baseNE, gray);
  float monoSW = dot(baseSW, gray);
  float monoSE = dot(baseSE, gray);
  float monomin = min(monocol, min(min(monoNW, monoNE), min(monoSW, monoSE)));
  float monomax = max(monocol, max(max(monoNW, monoNE), max(monoSW, monoSE)));
  vec2 dir = vec2(-((monoNW + monoNE) - (monoSW + monoSE)), ((monoNW + monoSW) - (monoNE + monoSE)));
  float dirreduce = max((monoNW + monoNE + monoSW + monoSE) * reducemul * 0.25, reducemin);
  float dirmin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirreduce);
  dir = min(vec2(strength), max(vec2(-strength), dir * dirmin)) * tsize;

  vec3 resultA = 0.5 * (texture2D(tex, uv + dir * -0.166667).rgb + texture2D(tex, uv + dir * 0.166667).rgb);
  vec3 resultB = resultA * 0.5 + 0.25 * (texture2D(tex, uv + dir * -0.5).rgb + texture2D(tex, uv + dir * 0.5).rgb);
  float monoB = dot(resultB, gray);

  if(monoB < monomin || monoB > monomax)
    return resultA;
  else
    return resultB;
}


in vec2 vUV0;
uniform sampler2D uSampler;
uniform vec2 TexelSize;
uniform float uFXAAStrength;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  FragColor.rgb = FXAA(uSampler, vUV0, TexelSize, uFXAAStrength);
}
