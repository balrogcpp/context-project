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

uniform sampler2D ColorMap;
uniform sampler2D FboMap;



//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(ColorMap, vUV0).rgb;
  vec3 rays = texture2D(FboMap, vUV0).rgb;
  FragColor.rgb = SafeHDR(color + rays);
}
