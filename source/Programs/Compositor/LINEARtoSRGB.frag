// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "srgb.glsl"


in mediump vec2 vUV0;
uniform sampler2D ColorMap;
uniform float Exposure;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(ColorMap, vUV0).rgb;
    color = expose(color, Exposure);
    FragColor = vec4(LINEARtoSRGB(color, Exposure), 1.0);
}
