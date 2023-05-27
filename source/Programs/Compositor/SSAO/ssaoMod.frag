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

SAMPLER2D(RT, 0);
SAMPLER2D(SsaoMap, 1);

OGRE_UNIFORMS_BEGIN
uniform vec4 ShadowColour;
OGRE_UNIFORMS_END

MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)
MAIN_DECLARATION
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump float ssao = texture2D(SsaoMap, vUV0).r;

    color *= clamp(ssao + ShadowColour.g, 0.0, 1.0);

    FragColor = vec4(SafeHDR(color), 1.0);
}
