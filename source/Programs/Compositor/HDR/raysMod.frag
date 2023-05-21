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
SAMPLER2D(FBO, 1);


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0);

MAIN_DECLARATION
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump vec3 rays = texture2D(FBO, vUV0).rgb;

    FragColor = vec4(SafeHDR(color + rays), 1.0);
}
