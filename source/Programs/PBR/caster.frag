// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
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
    if (texture2D(uAlbedoSampler, vUV0.xy).a < SurfaceAlphaRejection) {
        discard;
    }
#endif

    FragColor.r = gl_FragCoord.z;
}
