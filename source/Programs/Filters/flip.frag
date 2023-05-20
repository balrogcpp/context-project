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
SAMPLER2D(RT, 0);


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(highp vec2 vUV0, TEXCOORD0)

MAIN_DECLARATION
{
    vec2 uv = vec2(vUV0.s, 1.0 - vUV0.t);
    FragColor = vec4(texture2D(RT, uv).rgb, 1.0);
}
