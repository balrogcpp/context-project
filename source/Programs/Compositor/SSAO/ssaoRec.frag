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

uniform sampler2D SsaoTex;
uniform sampler2D SsaoTex2;

in highp vec2 vUV0;
void main()
{
    FragColor.r = SafeHDR(min(texture2D(SsaoTex, vUV0).r, texture2D(SsaoTex2, vUV0).r));
}
