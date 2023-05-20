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
#include "filters_RGB16.glsl"
uniform sampler2D RT;
uniform mediump vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(highp vec2 vUV0, TEXCOORD0)

MAIN_DECLARATION
{
    FragColor = vec4(Upscale9(RT, vUV0, TexelSize0), 1.0);
}
