// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D FBO;
uniform vec2 TexelSize;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale9(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 A = textureLod(tex, uv + tsize * vec2(-1.0, -1.0), 0.0).xyz;
    vec3 B = textureLod(tex, uv + tsize * vec2( 0.0, -1.0), 0.0).xyz;
    vec3 C = textureLod(tex, uv + tsize * vec2( 1.0, -1.0), 0.0).xyz;
    vec3 D = textureLod(tex, uv + tsize * vec2(-1.0,  0.0), 0.0).xyz;
    vec3 E = textureLod(tex, uv + tsize                   , 0.0).xyz;
    vec3 F = textureLod(tex, uv + tsize * vec2( 1.0,  0.0), 0.0).xyz;
    vec3 G = textureLod(tex, uv + tsize * vec2(-1.0,  1.0), 0.0).xyz;
    vec3 H = textureLod(tex, uv + tsize * vec2( 0.0,  1.0), 0.0).xyz;
    vec3 I = textureLod(tex, uv + tsize * vec2( 1.0,  1.0), 0.0).xyz;

    vec3 c1 = E * 0.25;
    vec3 c2 = (B + D + F + H) * 0.125;
    vec3 c3 = (A + C + G + I) * 0.0625;

    return c1 + c2 + c3;
}

in highp vec2 vUV0;
void main()
{
    vec3 color = textureLod(RT, vUV0, 0.0).rgb;
    vec3 fbo = Upscale9(FBO, vUV0, TexelSize).rgb;

    FragColor.rgb = SafeHDR(color + fbo / 6.0);
}
