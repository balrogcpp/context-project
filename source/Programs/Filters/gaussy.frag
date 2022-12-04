// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "filters.glsl"


in vec2 vUV0;
uniform sampler2D uSampler;
uniform vec2 TexelSize;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  FragColor.rgb = Gauss9V(uSampler, vUV0, TexelSize);
}
