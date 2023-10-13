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

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
mediump float Downscale13(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).x;
    mediump float B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).x;
    mediump float C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).x;
    mediump float D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).x;
    mediump float E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).x;
    mediump float F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).x;
    mediump float G = texture2D(tex, uv                           ).x;
    mediump float H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).x;
    mediump float I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).x;
    mediump float J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).x;
    mediump float K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).x;
    mediump float L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).x;
    mediump float M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).x;

    mediump float c1 = (D + E + I + J) * 0.125;
    mediump float c2 = (A + B + G + F) * 0.03125;
    mediump float c3 = (B + C + H + G) * 0.03125;
    mediump float c4 = (F + G + L + K) * 0.03125;
    mediump float c5 = (G + H + M + L) * 0.03125;

    return c1 + c2 + c3 + c4 + c5;
}

in highp vec2 vUV0;
void main()
{
    FragColor = vec4(Downscale13(RT, vUV0, TexelSize0), 0.0, 0.0, 1.0);
}
