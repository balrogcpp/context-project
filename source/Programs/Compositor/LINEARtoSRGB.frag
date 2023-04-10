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
    vec3 color = texture2D(ColorMap, vUV0).rgb;
    color = expose(color, Exposure);
    FragColor.rgb = LINEARtoSRGB(color, Exposure);
}
