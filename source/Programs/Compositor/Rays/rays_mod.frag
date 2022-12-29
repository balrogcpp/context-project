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


in vec2 vUV0;

uniform sampler2D uSampler;
uniform sampler2D uFBO;
uniform vec2 TexelSize1;


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
void main()
{
  vec3 color = texture2D(uSampler, vUV0).rgb;
  //vec3 rays = texture2D(uFBO, vUV0).rgb;
  vec3 rays = Upscale3x3(uFBO, vUV0, TexelSize1);
  FragColor.rgb = SafeHDR(color + rays);
}
