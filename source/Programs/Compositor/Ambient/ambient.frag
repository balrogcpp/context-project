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
uniform sampler2D uColorMap;
uniform sampler2D uAmbientMap;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uColorMap, vUV0).rgb;
  vec3 ambient = texture2D(uAmbientMap, vUV0).rgb;
  FragColor.rgb = color + ambient;
}
