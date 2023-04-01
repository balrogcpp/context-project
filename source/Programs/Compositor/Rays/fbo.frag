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



//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uSampler, vUV0).rgb;
  color /= (1.0 + luminance((color)));
  float depth = texture2D(uDepth, vUV0).w;
  FragColor.rgb = bigger(depth, 0.5) * color;
}
