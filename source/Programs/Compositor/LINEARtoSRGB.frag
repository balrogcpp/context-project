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
uniform sampler2D ColorMap;
uniform float Exposure;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    vec2 nuv = vec2(vUV0.s, 1.0 - vUV0.t);
    vec3 color = texture2D(ColorMap, nuv).rgb;
    color = expose(color, Exposure);
    FragColor.rgb = LINEARtoSRGB(color, Exposure);
}
