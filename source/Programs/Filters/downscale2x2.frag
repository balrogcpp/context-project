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


in mediump vec2 vUV0;
uniform mediump sampler2D RT;
uniform mediump vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    FragColor.rgb = Downscale2x2(RT, vUV0, TexelSize0);;
}
