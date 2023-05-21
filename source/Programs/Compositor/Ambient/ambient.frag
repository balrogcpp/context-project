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
SAMPLER2D(AmbientMap, 1);


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0);

MAIN_DECLARATION
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump vec3 ambient = texture2D(AmbientMap, vUV0).rgb;

    FragColor = vec4(SafeHDR(color + ambient), 1.0);
}
