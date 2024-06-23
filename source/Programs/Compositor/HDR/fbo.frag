// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

uniform vec2 TexelSize;

// https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

vec3 ToSRGB(const vec3 v)
{
    return sqrt(v);
}

float RGBToLuminance(const vec3 col)
{
    return dot(col, vec3(0.2126, 0.7152, 0.0722));
}

float KarisAverage(const vec3 col)
{
    // Formula is 1 / (1 + luma)
    float luma = RGBToLuminance(ToSRGB(col)) * 0.25;
    return 1.0 / (1.0 + luma);
}

// Karis filter based on
// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15

in highp vec2 vUV0;
void main()
{
    vec3 A = textureLod(RT, vUV0 + TexelSize * vec2(-1.0, -1.0), 0.0).rgb;
    vec3 B = textureLod(RT, vUV0 + TexelSize * vec2( 0.0, -1.0), 0.0).rgb;
    vec3 C = textureLod(RT, vUV0 + TexelSize * vec2( 1.0, -1.0), 0.0).rgb;
    vec3 D = textureLod(RT, vUV0 + TexelSize * vec2(-0.5, -0.5), 0.0).rgb;
    vec3 E = textureLod(RT, vUV0 + TexelSize * vec2( 0.5, -0.5), 0.0).rgb;
    vec3 F = textureLod(RT, vUV0 + TexelSize * vec2(-1.0,  0.0), 0.0).rgb;
    vec3 G = textureLod(RT, vUV0                               , 0.0).rgb;
    vec3 H = textureLod(RT, vUV0 + TexelSize * vec2( 1.0,  0.0), 0.0).rgb;
    vec3 I = textureLod(RT, vUV0 + TexelSize * vec2(-0.5,  0.5), 0.0).rgb;
    vec3 J = textureLod(RT, vUV0 + TexelSize * vec2( 0.5,  0.5), 0.0).rgb;
    vec3 K = textureLod(RT, vUV0 + TexelSize * vec2(-1.0,  1.0), 0.0).rgb;
    vec3 L = textureLod(RT, vUV0 + TexelSize * vec2( 0.0,  1.0), 0.0).rgb;
    vec3 M = textureLod(RT, vUV0 + TexelSize * vec2( 1.0,  1.0), 0.0).rgb;

    vec3 c1 = (D + E + I + J) * 0.125;
    vec3 c2 = (A + B + G + F) * 0.03125;
    vec3 c3 = (B + C + H + G) * 0.03125;
    vec3 c4 = (F + G + L + K) * 0.03125;
    vec3 c5 = (G + H + M + L) * 0.03125;

    vec3 c = c1 * KarisAverage(c1);
    c += c2 * KarisAverage(c2);
    c += c3 * KarisAverage(c3);
    c += c4 * KarisAverage(c4);
    c += c5 * KarisAverage(c5);

    FragColor.rgb = max(c, 0.0001);
}
