// created by Andrey Vasiliev

#ifndef FILTERS_GLSL
#define FILTERS_GLSL


#include "srgb.glsl"
#include "tonemap.glsl"


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Linear(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    return texture2D(tex, uv).xyz;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 BoxFilter4(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).xyz;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).xyz;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).xyz;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).xyz;

    mediump vec3 color = (A + B + C + D) * 0.25;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 BoxFilter9(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
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

    mediump vec3 color = (A + B + C) * 0.111111111111111;
    color += (D + E + F) * 0.111111111111111;
    color += (G + H + I) * 0.111111111111111;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 BoxFilter16(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).xyz;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).xyz;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).xyz;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2(-2.0, 1.0))).xyz;
    mediump vec3 E = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).xyz;
    mediump vec3 F = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).xyz;
    mediump vec3 G = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).xyz;
    mediump vec3 H = texture2D(tex, uv + (tsize * vec2(-1.0, 1.0))).xyz;
    mediump vec3 I = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).xyz;
    mediump vec3 J = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).xyz;
    mediump vec3 K = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).xyz;
    mediump vec3 L = texture2D(tex, uv + (tsize * vec2(0.0, 1.0))).xyz;
    mediump vec3 M = texture2D(tex, uv + (tsize * vec2(1.0, -2.0))).xyz;
    mediump vec3 N = texture2D(tex, uv + (tsize * vec2(1.0, -1.0))).xyz;
    mediump vec3 O = texture2D(tex, uv + (tsize * vec2(1.0, 0.0))).xyz;
    mediump vec3 P = texture2D(tex, uv + (tsize * vec2(1.0, 1.0))).xyz;
    
    mediump vec3 color = (A + B + C + D) * 0.0625;
    color += (E + F + G + H) * 0.0625;
    color += (I + J + K + L) * 0.0625;
    color += (M + N + O + P) * 0.0625;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss5V(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3333333333333333)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3333333333333333)).xyz;

    mediump vec3 color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss5H(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(1.3333333333333333, 0.0)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(1.3333333333333333, 0.0)).xyz;

    mediump vec3 color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss9V(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).xyz;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).xyz;

    mediump vec3 color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss9H(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).xyz;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).xyz;

    mediump vec3 color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss13V(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.411764705882353)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.411764705882353)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2941176470588234)).xyz;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2941176470588234)).xyz;
    mediump vec3 F = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).xyz;
    mediump vec3 G = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).xyz;

    mediump vec3 color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss13H(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(1.411764705882353, 0.0)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(1.411764705882353, 0.0)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(3.2941176470588234, 0.0)).xyz;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(3.2941176470588234, 0.0)).xyz;
    mediump vec3 F = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).xyz;
    mediump vec3 G = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).xyz;

    mediump vec3 color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
mediump vec3 Upscale3x3(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
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

    mediump vec3 color = E * 0.25;
    color += (B + D + F + H) * 0.125;
    color += (A + C + G + I) * 0.0625;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Upscale2x2(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).xyz;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-0.5,  0.5))).xyz;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2( 0.5, -0.5))).xyz;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2( 0.5,  0.5))).xyz;

    mediump vec3 color = (A + B + C + D) * 0.25; // 1/4

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Downscale2x2(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).xyz;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-0.5,  0.5))).xyz;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2( 0.5, -0.5))).xyz;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2( 0.5,  0.5))).xyz;

    mediump vec3 color = (A + B + C + D) * 0.25; // 1/4

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
mediump vec3 Downscale4x4(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
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

    mediump vec3 color = (D + E + I + J) * 0.125;
    color += (A + B + G + F) * 0.03125;
    color += (B + C + H + G) * 0.03125;
    color += (F + G + L + K) * 0.03125;
    color += (G + H + M + L) * 0.03125;

    return color;
}


#endif // FILTERS_GLSL
