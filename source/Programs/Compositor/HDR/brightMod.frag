// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "filters_RGB16.glsl"


varying mediump vec2 vUV0;
uniform sampler2D RT;
uniform sampler2D RT0;
uniform mediump vec2 TexelSize1;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 rt = texture2D(RT, vUV0).rgb;
    mediump vec3 rt0 = Upscale9(RT0, vUV0, TexelSize1);
    const mediump float w = 1.0 / 10.0;

    FragColor = vec4(SafeHDR(rt + rt0 * w), 1.0);
}
