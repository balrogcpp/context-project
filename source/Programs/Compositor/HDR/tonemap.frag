// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "srgb.glsl"
#include "tonemap.glsl"


varying mediump vec2 vUV0;
uniform sampler2D RT;


// https://github.com/OGRECave/ogre-next/blob/v2.3.1/Samples/Media/2.0/scripts/materials/HDR/GLSL/FinalToneMapping_ps.glsl
//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;

    color = uncharted2(color);
    color  = (color - 0.5) * 1.25 + 0.5 + 0.11;

    FragColor = vec4(SafeHDR(color), 1.0);
}
