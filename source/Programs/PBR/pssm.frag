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
varying mediump vec2 vUV0;
#endif


//----------------------------------------------------------------------------------------------------------------------
mediump float map_0(const mediump float x, const mediump float v0, const mediump float v1)
{
    return (x - v0) / (v1 - v0);
}


//----------------------------------------------------------------------------------------------------------------------
mediump float map_1(const mediump float x, const mediump float v0, const mediump float v1)
{
    return x * (v1 - v0) + v0;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(AlbedoMap, vUV0.xy).a < SurfaceAlphaRejection) {
        discard;
    }
#endif

    FragColor = vec4(map_0(gl_FragCoord.z, 0.0, 100.0), 0.0, 0.0, 1.0);
}
