// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "filters.glsl"
#include "math.glsl"


in vec2 vUV0;
uniform sampler2D uRT;
uniform sampler2D uRT0;
//uniform sampler2D uStarburst;
uniform vec2 TexelSize1;
uniform float mipCount;


//----------------------------------------------------------------------------------------------------------------------
float SampleStarburst(const sampler2D tex, const vec2 _uv, const float offset)
{
  vec2 uv = _uv - vec2(0.5);
  float d = length(uv);
  float radial = acos(uv.x / d);
  float mask = texture2D(tex, vec2(radial + offset * 1.0, 0.0)).r * texture2D(tex, vec2(radial - offset * 0.5, 0.0)).r;
  mask = saturate(mask + (1.0 - smoothstep(0.0, 0.3, d)));

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
