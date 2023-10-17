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
uniform sampler2D AlbedoTex;
#endif

#ifdef SHADOWCASTER_ALPHA
in highp vec2 vUV0;
#endif
void main()
{
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(AlbedoTex, vUV0.xy).a < 0.5) {
        discard;
    }
#endif

    FragColor.r = map_0(gl_FragCoord.z, 0.1, 100.0);
}
