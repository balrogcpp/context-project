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
uniform sampler2D Lum;
uniform sampler2D DepthTex;
uniform mediump vec2 TexelSize0;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
mediump vec3 Downscale13(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).xyz;
    mediump vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).xyz;
    mediump vec3 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).xyz;
    mediump vec3 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).xyz;
    mediump vec3 G = texture2D(tex, uv                           ).xyz;
    mediump vec3 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).xyz;
    mediump vec3 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).xyz;
    mediump vec3 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).xyz;
    mediump vec3 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).xyz;
    mediump vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).xyz;
    mediump vec3 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).xyz;

    mediump vec3 c1 = (D + E + I + J) * 0.125;
    mediump vec3 c2 = (A + B + G + F) * 0.03125;
    mediump vec3 c3 = (B + C + H + G) * 0.03125;
    mediump vec3 c4 = (F + G + L + K) * 0.03125;
    mediump vec3 c5 = (G + H + M + L) * 0.03125;

    return c1 + c2 + c3 + c4 + c5;
}

in highp vec2 vUV0;
void main()
{
    mediump vec3 color = Downscale13(RT, vUV0, TexelSize0).rgb;
    mediump float lum = texture2D(Lum, vec2(0.0, 0.0)).r;
    mediump float depth = texture2D(DepthTex, vUV0).x;

    //color *= lum;

    FragColor = vec4(bigger(depth, 0.5) * color, 1.0);
}
