// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
vec3 Downscale13(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 A = textureLod(tex, uv + tsize * vec2(-1.0, -1.0), 0.0).rgb;
    vec3 B = textureLod(tex, uv + tsize * vec2( 0.0, -1.0), 0.0).rgb;
    vec3 C = textureLod(tex, uv + tsize * vec2( 1.0, -1.0), 0.0).rgb;
    vec3 D = textureLod(tex, uv + tsize * vec2(-0.5, -0.5), 0.0).rgb;
    vec3 E = textureLod(tex, uv + tsize * vec2( 0.5, -0.5), 0.0).rgb;
    vec3 F = textureLod(tex, uv + tsize * vec2(-1.0,  0.0), 0.0).rgb;
    vec3 G = textureLod(tex, uv                           , 0.0).rgb;
    vec3 H = textureLod(tex, uv + tsize * vec2( 1.0,  0.0), 0.0).rgb;
    vec3 I = textureLod(tex, uv + tsize * vec2(-0.5,  0.5), 0.0).rgb;
    vec3 J = textureLod(tex, uv + tsize * vec2( 0.5,  0.5), 0.0).rgb;
    vec3 K = textureLod(tex, uv + tsize * vec2(-1.0,  1.0), 0.0).rgb;
    vec3 L = textureLod(tex, uv + tsize * vec2( 0.0,  1.0), 0.0).rgb;
    vec3 M = textureLod(tex, uv + tsize * vec2( 1.0,  1.0), 0.0).rgb;

    vec3 c = (D + E + I + J) * 0.125;
    c += (A + B + G + F) * 0.03125;
    c += (B + C + H + G) * 0.03125;
    c += (F + G + L + K) * 0.03125;
    c += (G + H + M + L) * 0.03125;

    return c;
}

void main()
{
    vec2 tsize = 1.0 / vec2(textureSize(RT, 0));
    vec2 uv = 2.0 * gl_FragCoord.xy * tsize;

    FragColor.rgb = Downscale13(RT, uv, tsize);
}
