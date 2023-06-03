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

OGRE_UNIFORMS_BEGIN
uniform mediump vec2 TexelSize0;
OGRE_UNIFORMS_END

MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)
MAIN_DECLARATION
{
    FragColor = vec4(Upscale9(RT, vUV0, TexelSize0), 1.0);
}
