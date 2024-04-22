// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec2 TexelSize;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale9(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
    vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).rgb;
    vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).rgb;
    vec3 D = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).rgb;
    vec3 E = texture2D(tex, uv + tsize                   ).rgb;
    vec3 F = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).rgb;
    vec3 G = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).rgb;
    vec3 H = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).rgb;
    vec3 I = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).rgb;

    vec3 c1 = E * 0.25;
    vec3 c2 = (B + D + F + H) * 0.125;
    vec3 c3 = (A + C + G + I) * 0.0625;

    return c1 + c2 + c3;
}

in highp vec2 vUV0;
void main()
{
    FragColor.rgb = Upscale9(RT, vUV0, TexelSize);
}
