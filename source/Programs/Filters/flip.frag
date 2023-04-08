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


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    vec2 uv = vec2(vUV0.s, 1.0 - vUV0.t);
    FragColor.rgb = texture2D(RT, uv).rgb;
}
