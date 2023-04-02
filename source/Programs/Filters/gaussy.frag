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
uniform sampler2D uRT;
uniform vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  FragColor.rgb = Gauss9V(uRT, vUV0, TexelSize0);
}
