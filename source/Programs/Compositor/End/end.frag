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


in mediump vec2 vUV0;
uniform sampler2D RT;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    FragColor = vec4(LINEARtoSRGB(color), 1.0);
}
