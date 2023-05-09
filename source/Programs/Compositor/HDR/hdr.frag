// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"


varying mediump vec2 vUV0;
uniform sampler2D RT;
uniform sampler2D Lum;
uniform sampler2D Bloom;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump vec3 bloom = texture2D(Bloom, vUV0).rgb;
    mediump float lum = texture2D(Lum, vec2(0.0, 0.0)).r;

    color *= lum;

    //color = uncharted2(color);
    //color = (color - 0.5) * 1.25 + 0.5 + 0.11;

    FragColor = vec4(SafeHDR(color), 1.0);
}
