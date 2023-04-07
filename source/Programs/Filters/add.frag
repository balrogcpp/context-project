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


in mediump vec2 vUV0;
uniform mediump sampler2D RT;
uniform mediump sampler2D RT0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 rt = texture2D(RT, vUV0).rgb;
    mediump vec3 rt0 = texture2D(RT0, vUV0).rgb;

    FragColor.rgb = rt + rt0;
}
