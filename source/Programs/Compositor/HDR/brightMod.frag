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
SAMPLER2D(RT0, 1);
uniform mediump vec2 TexelSize1;


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)

MAIN_DECLARATION
{
    mediump vec3 rt = texture2D(RT, vUV0).rgb;
    mediump vec3 rt0 = Upscale9(RT0, vUV0, TexelSize1);
    const mediump float w = 1.0 / 10.0;

    FragColor = vec4(SafeHDR(rt + rt0 * w), 1.0);
}
