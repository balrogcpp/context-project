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

mediump float Downscale4x4(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + tsize * vec2(-2.0, -2.0)).x;
    mediump float B = texture2D(tex, uv + tsize * vec2(-2.0, -1.0)).x;
    mediump float C = texture2D(tex, uv + tsize * vec2(-2.0,  0.0)).x;
    mediump float D = texture2D(tex, uv + tsize * vec2(-2.0,  1.0)).x;
    mediump float E = texture2D(tex, uv + tsize * vec2(-1.0, -2.0)).x;
    mediump float F = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).x;
    mediump float G = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).x;
    mediump float H = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).x;
    mediump float I = texture2D(tex, uv + tsize * vec2( 0.0, -2.0)).x;
    mediump float J = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).x;
    mediump float K = texture2D(tex, uv + tsize * vec2( 0.0,  0.0)).x;
    mediump float L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).x;
    mediump float M = texture2D(tex, uv + tsize * vec2( 1.0, -2.0)).x;
    mediump float N = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).x;
    mediump float O = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).x;
    mediump float P = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).x;

    mediump float c1 = (A + B + C + D) * 0.0625;
    mediump float c2 = (E + F + G + H) * 0.0625;
    mediump float c3 = (I + J + K + L) * 0.0625;
    mediump float c4 = (M + N + O + P) * 0.0625;

    return c1 + c2 + c3 + c4;
}

in highp vec2 vUV0;
void main()
{
    FragColor = vec4(Downscale4x4(RT, vUV0, TexelSize0), 0.0, 0.0, 1.0);
}
