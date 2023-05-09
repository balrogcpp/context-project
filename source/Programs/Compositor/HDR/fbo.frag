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
uniform sampler2D Lum;
uniform sampler2D DepthMap;
uniform mediump vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = Downscale13(RT, vUV0, TexelSize0).rgb;
    mediump float lum = texture2D(Lum, vec2(0.0, 0.0)).r;
    mediump float depth = texture2D(DepthMap, vUV0).x;

    color *= lum;

    FragColor = vec4(bigger(depth, 0.5) * color, 1.0);
}
