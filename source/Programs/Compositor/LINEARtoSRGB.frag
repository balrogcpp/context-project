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


in vec2 vUV0;
uniform sampler2D uColorMap;
uniform float uExposure;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uColorMap, vUV0).rgb;
  color = expose(color, uExposure);
  FragColor.rgb = LINEARtoSRGB(color, uExposure);
}
