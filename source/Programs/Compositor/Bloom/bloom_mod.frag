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
uniform sampler2D uRT;
uniform sampler2D uRT0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 rt = texture2D(uRT, vUV0).rgb;
  vec3 rt0 = texture2D(uRT0, vUV0).rgb;

  FragColor.rgb = rt + rt0 * (1.0 / 10.0);
}
