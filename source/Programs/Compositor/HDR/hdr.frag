// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D BrightTex;
uniform sampler2D Lum;

uniform vec2 TexelSize;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale9(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec4 d = tsize.xyxy * vec4(1.0, 1.0, -1.0, 0.0);

    vec3 A = textureLod(tex, uv - d.xy, 0.0).rgb;
    vec3 B = textureLod(tex, uv - d.wy, 0.0).rgb;
    vec3 C = textureLod(tex, uv - d.zy, 0.0).rgb;
    vec3 D = textureLod(tex, uv + d.zw, 0.0).rgb;
    vec3 E = textureLod(tex, uv       , 0.0).rgb;
    vec3 F = textureLod(tex, uv + d.xw, 0.0).rgb;
    vec3 G = textureLod(tex, uv + d.zy, 0.0).rgb;
    vec3 H = textureLod(tex, uv + d.wy, 0.0).rgb;
    vec3 I = textureLod(tex, uv + d.xy, 0.0).rgb;

    vec3 c = E * 0.25;
    c += (B + D + F + H) * 0.125;
    c += (A + C + G + I) * 0.0625;

    return c;
}

in highp vec2 vUV0;
void main()
{
    float lum = texelFetch(Lum, ivec2(0, 0), 0).r;
    vec3 bright = Upscale9(BrightTex, vUV0, TexelSize).rgb;
    vec3 color = textureLod(RT, vUV0, 0.0).rgb;
    color = mix(color, bright, 0.04);
    FragColor.rgb = unreal(color * lum);
}
