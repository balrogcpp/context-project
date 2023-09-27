// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D SsaoMap;

uniform vec4 ShadowColour;

in mediump vec2 vUV0;
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump float ssao = texture2D(SsaoMap, vUV0).r;

    color *= clamp(ssao + ShadowColour.g, 0.0, 1.0);
    FragColor = vec4(SafeHDR(color), 1.0);
}
