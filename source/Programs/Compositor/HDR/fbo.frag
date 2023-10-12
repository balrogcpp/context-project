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
#include "filters_RGB16.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;
uniform sampler2D DepthMap;
uniform mediump vec2 TexelSize0;

in highp vec2 vUV0;
void main()
{
    mediump vec3 color = Downscale13(RT, vUV0, TexelSize0).rgb;
    mediump float lum = texture2D(Lum, vec2(0.0, 0.0)).r;
    mediump float depth = texture2D(DepthMap, vUV0).x;

    //color *= lum;

    FragColor = vec4(bigger(depth, 0.5) * color, 1.0);
}
