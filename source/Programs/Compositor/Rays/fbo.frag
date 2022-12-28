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
#include "filters.glsl"


in vec2 vUV0;
uniform sampler2D uSampler;
uniform sampler2D uDepth;
uniform float FarClipDistance;
uniform vec2 TexelSize0;



//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uSampler, vUV0).rgb;
  //vec3 color = Downscale4x4(uSampler, vUV0, TexelSize0);
  float depth = texture2D(uDepth, vUV0).w;
  FragColor.rgb = bigger(depth, 0.5) * color;
}
