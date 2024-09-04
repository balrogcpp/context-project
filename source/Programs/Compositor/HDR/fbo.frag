// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

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
void main()
{
    vec2 size = vec2(textureSize(RT, 0));
    vec2 tsize = 1.0 / size;
    vec2 uv = 2.0 * gl_FragCoord.xy / size;
    uv.y = 1.0 - uv.y;

    vec3 A = textureLod(RT, uv + tsize * vec2(-1.0, -1.0), 0.0).rgb;
    vec3 B = textureLod(RT, uv + tsize * vec2( 0.0, -1.0), 0.0).rgb;
    vec3 C = textureLod(RT, uv + tsize * vec2( 1.0, -1.0), 0.0).rgb;
    vec3 D = textureLod(RT, uv + tsize * vec2(-0.5, -0.5), 0.0).rgb;
    vec3 E = textureLod(RT, uv + tsize * vec2( 0.5, -0.5), 0.0).rgb;
    vec3 F = textureLod(RT, uv + tsize * vec2(-1.0,  0.0), 0.0).rgb;
    vec3 G = textureLod(RT, uv                           , 0.0).rgb;
    vec3 H = textureLod(RT, uv + tsize * vec2( 1.0,  0.0), 0.0).rgb;
    vec3 I = textureLod(RT, uv + tsize * vec2(-0.5,  0.5), 0.0).rgb;
    vec3 J = textureLod(RT, uv + tsize * vec2( 0.5,  0.5), 0.0).rgb;
    vec3 K = textureLod(RT, uv + tsize * vec2(-1.0,  1.0), 0.0).rgb;
    vec3 L = textureLod(RT, uv + tsize * vec2( 0.0,  1.0), 0.0).rgb;
    vec3 M = textureLod(RT, uv + tsize * vec2( 1.0,  1.0), 0.0).rgb;

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
