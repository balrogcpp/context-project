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
#include "filters.glsl"


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Threshold(const mediump vec3 color, const mediump float threshold)
{
    return max(color - vec3(threshold), vec3(0.0, 0.0, 0.0));
}


in vec2 vUV0;
uniform sampler2D ColorMap;
uniform mediump vec2 TexelSize0;
uniform mediump float Threshhold;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = Downscale13T(ColorMap, vUV0, TexelSize0);
    FragColor = vec4(Threshold(color, Threshhold), 1.0);
}
