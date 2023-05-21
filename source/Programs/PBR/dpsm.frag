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
uniform highp float SurfaceAlphaRejection;
SAMPLER2D(AlbedoMap, 0);
#endif


//----------------------------------------------------------------------------------------------------------------------
highp float map_0(const highp float x, const highp float v0, const highp float v1)
{
    return (x - v0) / (v1 - v0);
}


//----------------------------------------------------------------------------------------------------------------------
highp float map_1(const highp float x, const highp float v0, const highp float v1)
{
    return x * (v1 - v0) + v0;
}


//----------------------------------------------------------------------------------------------------------------------
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
