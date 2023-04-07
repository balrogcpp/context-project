// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"


in mediump vec2 vUV0;
uniform mediump sampler2D ColorMap;
uniform mediump sampler2D SsaoMap;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(ColorMap, vUV0).rgb;
    mediump float ssao = texture2D(SsaoMap, vUV0).r;
    color *= clamp(ssao, 0.0, 1.0);
    FragColor.rgb = color;
}
