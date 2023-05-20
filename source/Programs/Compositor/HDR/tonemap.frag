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


// https://github.com/OGRECave/ogre-next/blob/v2.3.1/Samples/Media/2.0/scripts/materials/HDR/GLSL/FinalToneMapping_ps.glsl
//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)

MAIN_DECLARATION
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;

    color = uncharted2(color);
    color = (color - 0.5) * 1.25 + 0.5 + 0.11;

    FragColor = vec4(SafeHDR(color), 1.0);
}
