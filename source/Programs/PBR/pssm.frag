// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif

#include "header.glsl"


#ifdef SHADOWCASTER_ALPHA
uniform sampler2D AlbedoMap;
uniform float SurfaceAlphaRejection;
in mediump vec2 vUV0;
#endif


void main()
{
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(AlbedoMap, vUV0.xy).a < SurfaceAlphaRejection) {
        discard;
    }
#endif

    FragColor = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
