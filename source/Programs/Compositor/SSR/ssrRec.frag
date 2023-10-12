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
#include "mosaic.glsl"

uniform sampler2D SsrTex;
uniform sampler2D SsrTex2;

in highp vec2 vUV0;
void main()
{
    FragColor = vec4(SafeHDR(max(texture2D(SsrTex, vUV0).rgb, texture2D(SsrTex2, vUV0).rgb)), 1.0);
}
