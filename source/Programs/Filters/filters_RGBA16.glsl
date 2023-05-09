// created by Andrey Vasiliev

#ifndef FILTERS_GLSL
#define FILTERS_GLSL


#include "srgb.glsl"


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Linear(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    return texture2D(tex, uv);
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Downscale3x3(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0)));
    mediump vec4 B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0)));
    mediump vec4 C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0)));
    mediump vec4 D = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0)));
    mediump vec4 E = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0)));
    mediump vec4 F = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0)));
    mediump vec4 G = texture2D(tex, uv + (tsize * vec2(0.0, -2.0)));
    mediump vec4 H = texture2D(tex, uv + (tsize * vec2(0.0, -1.0)));
    mediump vec4 I = texture2D(tex, uv + (tsize * vec2(0.0, 0.0)));

    mediump vec4 c1 = (A + B + C) * 0.111111111111111;
    mediump vec4 c2 = (D + E + F) * 0.111111111111111;
    mediump vec4 c3 = (G + H + I) * 0.111111111111111;

    return c1 + c2 + c3;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Downscale4x4(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0)));
    mediump vec4 B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0)));
    mediump vec4 C = texture2D(tex, uv + (tsize * vec2(-2.0,  0.0)));
    mediump vec4 D = texture2D(tex, uv + (tsize * vec2(-2.0,  1.0)));
    mediump vec4 E = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0)));
    mediump vec4 F = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0)));
    mediump vec4 G = texture2D(tex, uv + (tsize * vec2(-1.0,  0.0)));
    mediump vec4 H = texture2D(tex, uv + (tsize * vec2(-1.0,  1.0)));
    mediump vec4 I = texture2D(tex, uv + (tsize * vec2( 0.0, -2.0)));
    mediump vec4 J = texture2D(tex, uv + (tsize * vec2( 0.0, -1.0)));
    mediump vec4 K = texture2D(tex, uv + (tsize * vec2( 0.0,  0.0)));
    mediump vec4 L = texture2D(tex, uv + (tsize * vec2( 0.0,  1.0)));
    mediump vec4 M = texture2D(tex, uv + (tsize * vec2( 1.0, -2.0)));
    mediump vec4 N = texture2D(tex, uv + (tsize * vec2( 1.0, -1.0)));
    mediump vec4 O = texture2D(tex, uv + (tsize * vec2( 1.0,  0.0)));
    mediump vec4 P = texture2D(tex, uv + (tsize * vec2( 1.0,  1.0)));

    mediump vec4 c1 = (A + B + C + D) * 0.0625;
    mediump vec4 c2 = (E + F + G + H) * 0.0625;
    mediump vec4 c3 = (I + J + K + L) * 0.0625;
    mediump vec4 c4 = (M + N + O + P) * 0.0625;

    return c1 + c2 + c3 + c4;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Gauss5V(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv);
    mediump vec4 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3333333333333333));
    mediump vec4 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3333333333333333));

    mediump vec4 c1 = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Gauss5H(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv);
    mediump vec4 B = texture2D(tex, uv + tsize * vec2(1.3333333333333333, 0.0));
    mediump vec4 C = texture2D(tex, uv - tsize * vec2(1.3333333333333333, 0.0));

    mediump vec4 c1 = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Gauss9V(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv);
    mediump vec4 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846));
    mediump vec4 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846));
    mediump vec4 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308));
    mediump vec4 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308));

    mediump vec4 c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Gauss9H(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv);
    mediump vec4 B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0));
    mediump vec4 C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0));
    mediump vec4 D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0));
    mediump vec4 E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0));

    mediump vec4 c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Gauss13V(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv);
    mediump vec4 B = texture2D(tex, uv + tsize * vec2(0.0, 1.411764705882353));
    mediump vec4 C = texture2D(tex, uv - tsize * vec2(0.0, 1.411764705882353));
    mediump vec4 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2941176470588234));
    mediump vec4 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2941176470588234));
    mediump vec4 F = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294));
    mediump vec4 G = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294));

    mediump vec4 c1 = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Gauss13H(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv);
    mediump vec4 B = texture2D(tex, uv + tsize * vec2(1.411764705882353, 0.0));
    mediump vec4 C = texture2D(tex, uv - tsize * vec2(1.411764705882353, 0.0));
    mediump vec4 D = texture2D(tex, uv + tsize * vec2(3.2941176470588234, 0.0));
    mediump vec4 E = texture2D(tex, uv - tsize * vec2(3.2941176470588234, 0.0));
    mediump vec4 F = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0));
    mediump vec4 G = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0));

    mediump vec4 c1 = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
mediump vec4 Upscale9(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0));
    mediump vec4 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0));
    mediump vec4 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0));
    mediump vec4 D = texture2D(tex, uv + tsize * vec2(-1.0,  0.0));
    mediump vec4 E = texture2D(tex, uv + tsize                   );
    mediump vec4 F = texture2D(tex, uv + tsize * vec2( 1.0,  0.0));
    mediump vec4 G = texture2D(tex, uv + tsize * vec2(-1.0,  1.0));
    mediump vec4 H = texture2D(tex, uv + tsize * vec2( 0.0,  1.0));
    mediump vec4 I = texture2D(tex, uv + tsize * vec2( 1.0,  1.0));

    mediump vec4 c1 = E * 0.25;
    mediump vec4 c2 = (B + D + F + H) * 0.125;
    mediump vec4 c3 = (A + C + G + I) * 0.0625;

    return c1 + c2 + c3;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Downscale2x2(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0)));
    mediump vec4 B = texture2D(tex, uv + (tsize * vec2(-1.0,  1.0)));
    mediump vec4 C = texture2D(tex, uv + (tsize * vec2( 1.0, -1.0)));
    mediump vec4 D = texture2D(tex, uv + (tsize * vec2( 1.0,  1.0)));

    mediump vec4 c1 = (A + B + C + D) * 0.25;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec4 Downscale2x2_05(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5)));
    mediump vec4 B = texture2D(tex, uv + (tsize * vec2(-0.5,  0.5)));
    mediump vec4 C = texture2D(tex, uv + (tsize * vec2( 0.5, -0.5)));
    mediump vec4 D = texture2D(tex, uv + (tsize * vec2( 0.5,  0.5)));

    mediump vec4 c1 = (A + B + C + D) * 0.25;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
mediump vec4 Downscale13(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec4 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0));
    mediump vec4 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0));
    mediump vec4 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0));
    mediump vec4 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5));
    mediump vec4 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5));
    mediump vec4 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0));
    mediump vec4 G = texture2D(tex, uv                           );
    mediump vec4 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0));
    mediump vec4 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5));
    mediump vec4 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5));
    mediump vec4 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0));
    mediump vec4 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0));
    mediump vec4 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0));

    mediump vec4 c1 = (D + E + I + J) * 0.125;
    mediump vec4 c2 = (A + B + G + F) * 0.03125;
    mediump vec4 c3 = (B + C + H + G) * 0.03125;
    mediump vec4 c4 = (F + G + L + K) * 0.03125;
    mediump vec4 c5 = (G + H + M + L) * 0.03125;

    return c1 + c2 + c3 + c4 + c5;
}


#endif // FILTERS_GLSL
