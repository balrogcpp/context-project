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
uniform sampler2D Lum;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump float lum = texture2D(Lum, vec2(0.0, 0.0)).r;

    color *= lum;

    FragColor = vec4(color, 1.0);
}
