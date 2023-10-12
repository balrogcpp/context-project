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
#include "filters_F16.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;
uniform mediump vec2 TexelSize0;
uniform mediump vec3 Exposure;
uniform mediump float timeSinceLast;

in highp vec2 vUV0;
void main()
{
    mediump float newLum = Downscale2x2(RT, vUV0, TexelSize0);
    newLum = expose2(newLum, Exposure);
    mediump float oldLum = texture2D(Lum, vec2(0.0, 0.0)).r;
    mediump float lum = mix(newLum, oldLum, pow(0.25, timeSinceLast));

    FragColor = vec4(SafeHDR(lum), 0.0, 0.0, 1.0);
}
