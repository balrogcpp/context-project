// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "math.glsl"


//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Upscale3x3(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, -1.0)).rgb;
    mediump vec3 C = texture2D(tex, uv + tsize * vec2(1.0, -1.0)).rgb;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(-1.0, 0.0)).rgb;
    mediump vec3 E = texture2D(tex, uv + tsize                  ).rgb;
    mediump vec3 F = texture2D(tex, uv + tsize * vec2(1.0, 0.0)).rgb;
    mediump vec3 G = texture2D(tex, uv + tsize * vec2(-1.0, 1.0)).rgb;
    mediump vec3 H = texture2D(tex, uv + tsize * vec2(0.0,  1.0)).rgb;
    mediump vec3 I = texture2D(tex, uv + tsize * vec2(1.0, 1.0)).rgb;

    mediump vec3 color = E * 0.25;
    color += (B + D + F + H) * 0.125;
    color += (A + C + G + I) * 0.0625;

    return color;
}


in vec2 vUV0;
uniform sampler2D RT;
uniform sampler2D RT0;
uniform mediump vec2 TexelSize1;
uniform mediump float MipCount;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 rt = texture2D(RT, vUV0).rgb;
    mediump vec3 rt0 = Upscale3x3(RT0, vUV0, TexelSize1);
    mediump float w = 1.0 / float(MipCount);
#ifdef GL_ES
    w *= 2.0;
#endif

    FragColor.rgb = SafeHDR(rt + rt0 * w);
}
