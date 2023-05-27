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
SAMPLER2D(AlbedoMap, 0);

OGRE_UNIFORMS_BEGIN
uniform highp float SurfaceAlphaRejection;
OGRE_UNIFORMS_END
#endif

MAIN_PARAMETERS
#ifdef SHADOWCASTER_ALPHA
IN(highp vec2 vUV0, TEXCOORD0)
#endif
MAIN_DECLARATION
{
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(AlbedoMap, vUV0.xy).a < SurfaceAlphaRejection) {
        discard;
    }
#endif

    FragColor = vec4(map_0(gl_FragCoord.z, 0.0, 100.0), 0.0, 0.0, 1.0);
}
