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
uniform sampler2D RT0;
uniform mediump vec2 TexelSize1;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
mediump vec3 Upscale9(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).xyz;
    mediump vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).xyz;
    mediump vec3 E = texture2D(tex, uv + tsize                   ).xyz;
    mediump vec3 F = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).xyz;
    mediump vec3 G = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).xyz;
    mediump vec3 H = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).xyz;
    mediump vec3 I = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).xyz;

    mediump vec3 c1 = E * 0.25;
    mediump vec3 c2 = (B + D + F + H) * 0.125;
    mediump vec3 c3 = (A + C + G + I) * 0.0625;

    return c1 + c2 + c3;
}

in highp vec2 vUV0;
void main()
{
    mediump vec3 rt = texture2D(RT, vUV0).rgb;
    mediump vec3 rt0 = Upscale9(RT0, vUV0, TexelSize1);
    const mediump float w = 1.0 / 8.0;

    FragColor.rgb = SafeHDR(rt + rt0 * w);
}
