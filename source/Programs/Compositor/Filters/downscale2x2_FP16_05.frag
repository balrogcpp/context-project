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

uniform sampler2D RT;
uniform mediump vec2 TexelSize0;

mediump float Downscale2x2_05(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).x;
    mediump float B = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).x;
    mediump float C = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).x;
    mediump float D = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).x;

    mediump float c1 = (A + B + C + D) * 0.25;

    return c1;
}

in highp vec2 vUV0;
void main()
{
    FragColor = vec4(Downscale2x2_05(RT, vUV0, TexelSize0), 0.0, 0.0, 1.0);
}
