// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec2 TexelSize;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L43
vec3 Box4(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 A = textureLod(tex, uv + tsize * vec2(-1.0, -1.0), 0.0).rgb;
    vec3 B = textureLod(tex, uv + tsize * vec2( 1.0, -1.0), 0.0).rgb;
    vec3 C = textureLod(tex, uv + tsize * vec2(-1.0,  1.0), 0.0).rgb;
    vec3 D = textureLod(tex, uv + tsize * vec2( 1.0,  1.0), 0.0).rgb;

    vec3 c1 = (A + B + C + D) * 0.25;

    return c1;
}

in highp vec2 vUV0;
void main()
{
    FragColor.rgb = Box4(RT, vUV0, TexelSize);
}
