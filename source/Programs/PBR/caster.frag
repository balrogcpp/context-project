// created by Andrey Vasiliev

#ifndef VERSION
#ifndef GL_ES
#version 150
#define VERSION 150
#else
#version 100
#define VERSION 100
#endif
#endif

#include "header.frag"


#ifdef SHADOWCASTER_ALPHA
uniform sampler2D uAlbedoSampler;
uniform float SurfaceAlphaRejection;
in vec2 vUV0;
#endif


void main()
{
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(uAlbedoSampler, vUV0.xy).a < SurfaceAlphaRejection) discard;
#endif

    FragColor.r = gl_FragCoord.z;
}
