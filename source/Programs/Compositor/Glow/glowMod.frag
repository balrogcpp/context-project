// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D RT0;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale9(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));
    vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).xyz;
    vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).xyz;
    vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).xyz;
    vec3 D = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).xyz;
    vec3 E = texture2D(tex, uv + tsize                   ).xyz;
    vec3 F = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).xyz;
    vec3 G = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).xyz;
    vec3 H = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).xyz;
    vec3 I = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).xyz;

    vec3 c1 = E * 0.25;
    vec3 c2 = (B + D + F + H) * 0.125;
    vec3 c3 = (A + C + G + I) * 0.0625;

    return c1 + c2 + c3;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(RT, 0));
    vec3 rt = texture2D(RT, uv).rgb;
    vec3 rt0 = texture2D(RT0, uv).rgb;
    FragColor.rgb = SafeHDR(rt + rt0 / 6.0);
}
