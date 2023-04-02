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
#include "filters.glsl"


//----------------------------------------------------------------------------------------------------------------------
vec3 Threshold(const vec3 color, const float threshold)
{
  return max(color - vec3(threshold), vec3(0.0));
}


in vec2 vUV0;
uniform sampler2D uColorMap;
uniform vec2 TexelSize0;
uniform float uThreshhold;



//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = Downscale13T(uColorMap, vUV0, TexelSize0);
  FragColor.rgb = Threshold(color, uThreshhold);
}
