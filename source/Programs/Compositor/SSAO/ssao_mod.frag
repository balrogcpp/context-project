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
uniform sampler2D ColorMap;
uniform sampler2D SsaoMap;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(ColorMap, vUV0).rgb;
  float ssao = texture2D(SsaoMap, vUV0).r;
  color *= clamp(ssao, 0.0, 1.0);
  FragColor.rgb = color;
}
