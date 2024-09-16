// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale9(const sampler2D tex, const vec2 uv)
{
    vec3 A = textureLodOffset(tex, uv, 0.0, ivec2(-1, -1)).rgb;
    vec3 B = textureLodOffset(tex, uv, 0.0, ivec2( 0, -1)).rgb;
    vec3 C = textureLodOffset(tex, uv, 0.0, ivec2( 1, -1)).rgb;
    vec3 D = textureLodOffset(tex, uv, 0.0, ivec2(-1,  1)).rgb;
    vec3 E = textureLodOffset(tex, uv, 0.0, ivec2( 0,  0)).rgb;
    vec3 F = textureLodOffset(tex, uv, 0.0, ivec2( 1,  0)).rgb;
    vec3 G = textureLodOffset(tex, uv, 0.0, ivec2(-1,  1)).rgb;
    vec3 H = textureLodOffset(tex, uv, 0.0, ivec2( 0,  1)).rgb;
    vec3 I = textureLodOffset(tex, uv, 0.0, ivec2( 1,  1)).rgb;

    vec3 o = E * 0.25;
    o += (B + D + F + H) * 0.125;
    o += (A + C + G + I) * 0.0625;

    return o;
}

void main()
{
    vec2 uv = 0.5 * gl_FragCoord.xy / vec2(textureSize(RT, 0));

    FragColor.rgb = Upscale9(RT, uv);
}
