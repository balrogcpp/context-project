// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L43
vec3 DownsampleBox4Tap(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 o;
    o =  textureLodOffset(tex, uv, 0.0, ivec2(-1, -1)).rgb;
    o += textureLodOffset(tex, uv, 0.0, ivec2( 1, -1)).rgb;
    o += textureLodOffset(tex, uv, 0.0, ivec2(-1,  1)).rgb;
    o += textureLodOffset(tex, uv, 0.0, ivec2( 1,  1)).rgb;

    return o * 0.25;
}

out vec3 FragColor;
void main()
{
    FragColor.rgb = DownsampleBox4Tap(RT, 2.0 * gl_FragCoord.xy / vec2(textureSize(RT, 0)));
}
