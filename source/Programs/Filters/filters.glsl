// created by Andrey Vasiliev

#ifndef FILTERS_GLSL
#define FILTERS_GLSL


#include "srgb.glsl"


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Linear(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    return texture2D(tex, uv).rgb;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 BoxFilter4(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).rgb;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).rgb;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).rgb;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).rgb;

    mediump vec3 color = (A + B + C + D) * 0.25;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float BoxFilter4R(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).r;
    mediump float B = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).r;
    mediump float C = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).r;
    mediump float D = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).r;

    mediump float color = (A + B + C + D) * 0.25;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 BoxFilter9(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).rgb;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).rgb;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).rgb;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).rgb;
    mediump vec3 E = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).rgb;
    mediump vec3 F = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).rgb;
    mediump vec3 G = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).rgb;
    mediump vec3 H = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).rgb;
    mediump vec3 I = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).rgb;

    mediump vec3 color = (A + B + C) * 0.111111111111111;
    color += (D + E + F) * 0.111111111111111;
    color += (G + H + I) * 0.111111111111111;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float BoxFilter9R(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).r;
    mediump float B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).r;
    mediump float C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).r;
    mediump float D = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).r;
    mediump float E = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).r;
    mediump float F = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).r;
    mediump float G = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).r;
    mediump float H = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).r;
    mediump float I = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).r;

    mediump float color = (A + B + C) * 0.111111111111111;
    color += (D + E + F) * 0.111111111111111;
    color += (G + H + I) * 0.111111111111111;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 BoxFilter16(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).rgb;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).rgb;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).rgb;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2(-2.0, 1.0))).rgb;
    mediump vec3 E = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).rgb;
    mediump vec3 F = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).rgb;
    mediump vec3 G = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).rgb;
    mediump vec3 H = texture2D(tex, uv + (tsize * vec2(-1.0, 1.0))).rgb;
    mediump vec3 I = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).rgb;
    mediump vec3 J = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).rgb;
    mediump vec3 K = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).rgb;
    mediump vec3 L = texture2D(tex, uv + (tsize * vec2(0.0, 1.0))).rgb;
    mediump vec3 M = texture2D(tex, uv + (tsize * vec2(1.0, -2.0))).rgb;
    mediump vec3 N = texture2D(tex, uv + (tsize * vec2(1.0, -1.0))).rgb;
    mediump vec3 O = texture2D(tex, uv + (tsize * vec2(1.0, 0.0))).rgb;
    mediump vec3 P = texture2D(tex, uv + (tsize * vec2(1.0, 1.0))).rgb;
    
    mediump vec3 color = (A + B + C + D) * 0.0625;
    color += (E + F + G + H) * 0.0625;
    color += (I + J + K + L) * 0.0625;
    color += (M + N + O + P) * 0.0625;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float BoxFilter16R(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).r;
    mediump float B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).r;
    mediump float C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).r;
    mediump float D = texture2D(tex, uv + (tsize * vec2(-2.0, 1.0))).r;
    mediump float E = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).r;
    mediump float F = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).r;
    mediump float G = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).r;
    mediump float H = texture2D(tex, uv + (tsize * vec2(-1.0, 1.0))).r;
    mediump float I = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).r;
    mediump float J = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).r;
    mediump float K = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).r;
    mediump float L = texture2D(tex, uv + (tsize * vec2(0.0, 1.0))).r;
    mediump float M = texture2D(tex, uv + (tsize * vec2(1.0, -2.0))).r;
    mediump float N = texture2D(tex, uv + (tsize * vec2(1.0, -1.0))).r;
    mediump float O = texture2D(tex, uv + (tsize * vec2(1.0, 0.0))).r;
    mediump float P = texture2D(tex, uv + (tsize * vec2(1.0, 1.0))).r;

    mediump float color = (A + B + C + D) * 0.0625;
    color += (E + F + G + H) * 0.0625;
    color += (I + J + K + L) * 0.0625;
    color += (M + N + O + P) * 0.0625;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss5V(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3333333333333333)).rgb;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3333333333333333)).rgb;

    mediump vec3 color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss5H(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(1.3333333333333333, 0.0)).rgb;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(1.3333333333333333, 0.0)).rgb;

    mediump vec3 color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Gauss5VR(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv).r;
    mediump float B = texture2D(tex, uv + tsize * vec2(0.0, 1.3333333333333333)).r;
    mediump float C = texture2D(tex, uv - tsize * vec2(0.0, 1.3333333333333333)).r;

    mediump float color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Gauss5HR(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv).r;
    mediump float B = texture2D(tex, uv + tsize * vec2(1.3333333333333333, 0.0)).r;
    mediump float C = texture2D(tex, uv - tsize * vec2(1.3333333333333333, 0.0)).r;

    mediump float color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss9V(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).rgb;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).rgb;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).rgb;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).rgb;

    mediump vec3 color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss9H(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).rgb;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).rgb;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).rgb;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).rgb;

    mediump vec3 color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Gauss9VR(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv).r;
    mediump float B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).r;
    mediump float C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).r;
    mediump float D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).r;
    mediump float E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).r;

    mediump float color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Gauss9HR(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv).r;
    mediump float B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).r;
    mediump float C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).r;
    mediump float D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).r;
    mediump float E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).r;

    mediump float color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss13V(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.411764705882353)).rgb;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.411764705882353)).rgb;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2941176470588234)).rgb;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2941176470588234)).rgb;
    mediump vec3 F = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).rgb;
    mediump vec3 G = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).rgb;

    mediump vec3 color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Gauss13H(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(1.411764705882353, 0.0)).rgb;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(1.411764705882353, 0.0)).rgb;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(3.2941176470588234, 0.0)).rgb;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(3.2941176470588234, 0.0)).rgb;
    mediump vec3 F = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).rgb;
    mediump vec3 G = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).rgb;

    mediump vec3 color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Gauss13VR(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv).r;
    mediump float B = texture2D(tex, uv + tsize * vec2(0.0, 1.411764705882353)).r;
    mediump float C = texture2D(tex, uv - tsize * vec2(0.0, 1.411764705882353)).r;
    mediump float D = texture2D(tex, uv + tsize * vec2(0.0, 3.2941176470588234)).r;
    mediump float E = texture2D(tex, uv - tsize * vec2(0.0, 3.2941176470588234)).r;
    mediump float F = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).r;
    mediump float G = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).r;

    mediump float color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Gauss13HR(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv).r;
    mediump float B = texture2D(tex, uv + tsize * vec2(1.411764705882353, 0.0)).r;
    mediump float C = texture2D(tex, uv - tsize * vec2(1.411764705882353, 0.0)).r;
    mediump float D = texture2D(tex, uv + tsize * vec2(3.2941176470588234, 0.0)).r;
    mediump float E = texture2D(tex, uv - tsize * vec2(3.2941176470588234, 0.0)).r;
    mediump float F = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).r;
    mediump float G = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).r;

    mediump float color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
mediump vec3 Upscale3x3(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
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


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
mediump float Upscale3x3R(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).r;
    mediump float B = texture2D(tex, uv + tsize * vec2(0.0, -1.0)).r;
    mediump float C = texture2D(tex, uv + tsize * vec2(1.0, -1.0)).r;
    mediump float D = texture2D(tex, uv + tsize * vec2(-1.0, 0.0)).r;
    mediump float E = texture2D(tex, uv + tsize                  ).r;
    mediump float F = texture2D(tex, uv + tsize * vec2(1.0, 0.0)).r;
    mediump float G = texture2D(tex, uv + tsize * vec2(-1.0, 1.0)).r;
    mediump float H = texture2D(tex, uv + tsize * vec2(0.0,  1.0)).r;
    mediump float I = texture2D(tex, uv + tsize * vec2(1.0, 1.0)).r;

    mediump float color = E * 0.25;
    color += (B + D + F + H) * 0.125;
    color += (A + C + G + I) * 0.0625;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Upscale2x2(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).rgb;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).rgb;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).rgb;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2(0.5, -0.5))).rgb;

    mediump vec3 color = (A + B + C + D) * 0.25; // 1/4

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Upscale2x2R(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).r;
    mediump float B = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).r;
    mediump float C = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).r;
    mediump float D = texture2D(tex, uv + (tsize * vec2(0.5, -0.5))).r;

    mediump float color = (A + B + C + D) * 0.25; // 1/4

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Downscale2x2(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).rgb;
    mediump vec3 B = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).rgb;
    mediump vec3 C = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).rgb;
    mediump vec3 D = texture2D(tex, uv + (tsize * vec2(0.5, -0.5))).rgb;

    mediump vec3 color = (A + B + C + D) * 0.25; // 1/4

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Downscale2x2R(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).r;
    mediump float B = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).r;
    mediump float C = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).r;
    mediump float D = texture2D(tex, uv + (tsize * vec2(0.5, -0.5))).r;

    mediump float color = (A + B + C + D) * 0.25; // 1/4

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
mediump vec3 Downscale4x4(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).rgb;
    mediump vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).rgb;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).rgb;
    mediump vec3 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).rgb;
    mediump vec3 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).rgb;
    mediump vec3 G = texture2D(tex, uv                           ).rgb;
    mediump vec3 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).rgb;
    mediump vec3 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).rgb;
    mediump vec3 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).rgb;
    mediump vec3 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).rgb;
    mediump vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).rgb;
    mediump vec3 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).rgb;

    mediump vec3 color = (D + E + I + J) * 0.125;
    color += (A + B + G + F) * 0.03125;
    color += (B + C + H + G) * 0.03125;
    color += (F + G + L + K) * 0.03125;
    color += (G + H + M + L) * 0.03125;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float Downscale4x4R(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).r;
    mediump float B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).r;
    mediump float C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).r;
    mediump float D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).r;
    mediump float E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).r;
    mediump float F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).r;
    mediump float G = texture2D(tex, uv                           ).r;
    mediump float H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).r;
    mediump float I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).r;
    mediump float J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).r;
    mediump float K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).r;
    mediump float L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).r;
    mediump float M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).r;

    mediump float color = (D + E + I + J) * 0.125;
    color += (A + B + G + F) * 0.03125;
    color += (B + C + H + G) * 0.03125;
    color += (F + G + L + K) * 0.03125;
    color += (G + H + M + L) * 0.03125;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Downscale13T(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).rgb;
    mediump vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).rgb;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).rgb;
    mediump vec3 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).rgb;
    mediump vec3 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).rgb;
    mediump vec3 G = texture2D(tex, uv                           ).rgb;
    mediump vec3 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).rgb;
    mediump vec3 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).rgb;
    mediump vec3 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).rgb;
    mediump vec3 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).rgb;
    mediump vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).rgb;
    mediump vec3 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).rgb;

    mediump vec3 c1 = (D + E + I + J); c1 /= (1.0 + luminance(LINEARtoSRGB(c1))); c1 *= 0.125;
    mediump vec3 c2 = (A + B + G + F); c2 /= (1.0 + luminance(LINEARtoSRGB(c2))); c2 *= 0.03125;
    mediump vec3 c3 = (B + C + H + G); c3 /= (1.0 + luminance(LINEARtoSRGB(c3))); c3 *= 0.03125;
    mediump vec3 c4 = (F + G + L + K); c4 /= (1.0 + luminance(LINEARtoSRGB(c4))); c4 *= 0.03125;
    mediump vec3 c5 = (G + H + M + L); c5 /= (1.0 + luminance(LINEARtoSRGB(c5))); c5 *= 0.03125;

    return c1 + c2 + c3 + c4 + c5;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Downscale13LUM(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).rgb;
    mediump vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).rgb;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).rgb;
    mediump vec3 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).rgb;
    mediump vec3 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).rgb;
    mediump vec3 G = texture2D(tex, uv                           ).rgb;
    mediump vec3 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).rgb;
    mediump vec3 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).rgb;
    mediump vec3 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).rgb;
    mediump vec3 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).rgb;
    mediump vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).rgb;
    mediump vec3 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).rgb;

    mediump vec3 c1 = (D + E + I + J); c1 /= (1.0 + luminance(c1)); c1 *= 0.125;
    mediump vec3 c2 = (A + B + G + F); c2 /= (1.0 + luminance(c2)); c2 *= 0.03125;
    mediump vec3 c3 = (B + C + H + G); c3 /= (1.0 + luminance(c3)); c3 *= 0.03125;
    mediump vec3 c4 = (F + G + L + K); c4 /= (1.0 + luminance(c4)); c4 *= 0.03125;
    mediump vec3 c5 = (G + H + M + L); c5 /= (1.0 + luminance(c5)); c5 *= 0.03125;

    return c1 + c2 + c3 + c4 + c5;
}



//----------------------------------------------------------------------------------------------------------------------
mediump vec3 FXAA(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize, const mediump float strength)
{
    #define reducemul 0.125 // 1/8
    #define reducemin 0.0078125 // 1/128
    #define gray vec3(0.299, 0.587, 0.114)

    mediump vec3 basecol = texture2D(tex, uv).rgb;
    mediump vec3 baseNW = texture2D(tex, uv - tsize).rgb;
    mediump vec3 baseNE = texture2D(tex, uv + vec2(tsize.x, -tsize.y)).rgb;
    mediump vec3 baseSW = texture2D(tex, uv + vec2(-tsize.x, tsize.y)).rgb;
    mediump vec3 baseSE = texture2D(tex, uv + tsize).rgb;
    mediump float monocol = dot(basecol, gray);
    mediump float monoNW = dot(baseNW, gray);
    mediump float monoNE = dot(baseNE, gray);
    mediump float monoSW = dot(baseSW, gray);
    mediump float monoSE = dot(baseSE, gray);
    mediump float monomin = min(monocol, min(min(monoNW, monoNE), min(monoSW, monoSE)));
    mediump float monomax = max(monocol, max(max(monoNW, monoNE), max(monoSW, monoSE)));
    mediump vec2 dir = vec2(-((monoNW + monoNE) - (monoSW + monoSE)), ((monoNW + monoSW) - (monoNE + monoSE)));
    mediump float dirreduce = max((monoNW + monoNE + monoSW + monoSE) * reducemul * 0.25, reducemin);
    mediump float dirmin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirreduce);
    dir = min(vec2(strength), max(vec2(-strength), dir * dirmin)) * tsize;

    mediump vec3 resultA = 0.5 * (texture2D(tex, uv + dir * -0.166667).rgb + texture2D(tex, uv + dir * 0.166667).rgb);
    mediump vec3 resultB = resultA * 0.5 + 0.25 * (texture2D(tex, uv + dir * -0.5).rgb + texture2D(tex, uv + dir * 0.5).rgb);
    mediump float monoB = dot(resultB, gray);

    if(monoB < monomin || monoB > monomax)
        return resultA;
    else
        return resultB;
}

#endif // FILTERS_GLSL
