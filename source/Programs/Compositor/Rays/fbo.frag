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
uniform sampler2D uColorMap;
uniform sampler2D uDepthMap;



//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uColorMap, vUV0).rgb;
  color /= (1.0 + luminance((color)));
  float depth = texture2D(uDepthMap, vUV0).x;
  FragColor.rgb = bigger(depth, 0.5) * color;
}
