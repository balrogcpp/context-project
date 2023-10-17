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
#include "srgb.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;

in highp vec2 vUV0;
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump float lum = clamp(texture2D(Lum, vec2(0.0, 0.0)).r, 0.0, 1.0);

    color *= lum;

//    color = uncharted2(color);
//    color = (color - 0.5) * 1.25 + 0.5 + 0.11;
//    FragColor.rgb = LINEARtoSRGB(SafeHDR(color));
    FragColor.rgb = SafeHDR(unreal(color));
}
