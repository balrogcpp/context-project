// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec2 TexelSize;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale9(const sampler2D tex, const vec2 uv, const vec2 tsize)
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
    FragColor.rgb = Upscale9(RT, vUV0, TexelSize);
}
