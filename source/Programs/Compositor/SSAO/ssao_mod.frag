// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"


in mediump vec2 vUV0;
uniform sampler2D RT;
uniform sampler2D SsaoMap;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump float ssao = texture2D(SsaoMap, vUV0).r;
    color *= ssao;
    FragColor = vec4(color, 1.0);
}
