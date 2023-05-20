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


#ifdef SHADOWCASTER_ALPHA
uniform sampler2D AlbedoMap;
uniform float SurfaceAlphaRejection;
IN(highp vec2 vUV0, TEXCOORD0)
#endif


MAIN_DECLARATION
{
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(AlbedoMap, vUV0.xy).a < SurfaceAlphaRejection) {
        discard;
    }
#endif

    FragColor = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
