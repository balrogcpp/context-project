// created by Andrey Vasiliev

#ifndef FILTERS_GLSL
#define FILTERS_GLSL


#include "srgb.glsl"

mediump vec3 Linear(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    return texture2D(tex, uv).xyz;
}

mediump vec3 Downscale3x3(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).xyz;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).xyz;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).xyz;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).xyz;
    mediump vec3 E = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).xyz;
    mediump vec3 F = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).xyz;
    mediump vec3 G = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).xyz;
    mediump vec3 H = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).xyz;
    mediump vec3 I = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).xyz;

    mediump vec3 c1 = (A + B + C) * 0.111111111111111;
    mediump vec3 c2 = (D + E + F) * 0.111111111111111;
    mediump vec3 c3 = (G + H + I) * 0.111111111111111;

    return c1 + c2 + c3;
}

mediump vec3 Downscale4x4(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).xyz;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).xyz;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2(-2.0,  0.0))).xyz;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2(-2.0,  1.0))).xyz;
    mediump vec3 E = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).xyz;
    mediump vec3 F = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).xyz;
    mediump vec3 G = texture2D(tex, uv + (tsize * vec2(-1.0,  0.0))).xyz;
    mediump vec3 H = texture2D(tex, uv + (tsize * vec2(-1.0,  1.0))).xyz;
    mediump vec3 I = texture2D(tex, uv + (tsize * vec2( 0.0, -2.0))).xyz;
    mediump vec3 J = texture2D(tex, uv + (tsize * vec2( 0.0, -1.0))).xyz;
    mediump vec3 K = texture2D(tex, uv + (tsize * vec2( 0.0,  0.0))).xyz;
    mediump vec3 L = texture2D(tex, uv + (tsize * vec2( 0.0,  1.0))).xyz;
    mediump vec3 M = texture2D(tex, uv + (tsize * vec2( 1.0, -2.0))).xyz;
    mediump vec3 N = texture2D(tex, uv + (tsize * vec2( 1.0, -1.0))).xyz;
    mediump vec3 O = texture2D(tex, uv + (tsize * vec2( 1.0,  0.0))).xyz;
    mediump vec3 P = texture2D(tex, uv + (tsize * vec2( 1.0,  1.0))).xyz;

    mediump vec3 c1 = (A + B + C + D) * 0.0625;
    mediump vec3 c2 = (E + F + G + H) * 0.0625;
    mediump vec3 c3 = (I + J + K + L) * 0.0625;
    mediump vec3 c4 = (M + N + O + P) * 0.0625;

    return c1 + c2 + c3 + c4;
}

mediump vec3 Gauss5V(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3333333333333333)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3333333333333333)).xyz;

    mediump vec3 c1 = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return c1;
}

mediump vec3 Gauss5H(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(1.3333333333333333, 0.0)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(1.3333333333333333, 0.0)).xyz;

    mediump vec3 c1 = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return c1;
}

mediump vec3 Gauss9V(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).xyz;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).xyz;

    mediump vec3 c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

mediump vec3 Gauss9H(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).xyz;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).xyz;

    mediump vec3 c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

mediump vec3 Gauss13V(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.411764705882353)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.411764705882353)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2941176470588234)).xyz;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2941176470588234)).xyz;
    mediump vec3 F = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).xyz;
    mediump vec3 G = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).xyz;

    mediump vec3 c1 = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return c1;
}

mediump vec3 Gauss13H(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(1.411764705882353, 0.0)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(1.411764705882353, 0.0)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(3.2941176470588234, 0.0)).xyz;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(3.2941176470588234, 0.0)).xyz;
    mediump vec3 F = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).xyz;
    mediump vec3 G = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).xyz;

    mediump vec3 c1 = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return c1;
}

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

mediump vec3 Downscale2x2(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).xyz;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-1.0,  1.0))).xyz;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2( 1.0, -1.0))).xyz;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2( 1.0,  1.0))).xyz;

    mediump vec3 c1 = (A + B + C + D) * 0.25;

    return c1;
}

mediump vec3 Downscale2x2_05(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).xyz;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-0.5,  0.5))).xyz;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2( 0.5, -0.5))).xyz;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2( 0.5,  0.5))).xyz;

    mediump vec3 c1 = (A + B + C + D) * 0.25;

    return c1;
}

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

#endif // FILTERS_GLSL
