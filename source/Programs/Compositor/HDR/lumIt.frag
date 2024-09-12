// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;


// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L43
float Downsample4(const sampler2D tex, const ivec2 uv)
{
#ifndef GL_ES
    vec4 g = textureGather(tex, uv / vec2(textureSize(tex, 0)), 0);
    float o = dot(g, vec4(1.0, 1.0, 1.0, 1.0));
#else
    float c = texelFetch(tex, uv, 0).r;
    o += texelFetch(tex, uv + ivec2(0, 1), 0).r;
    o += texelFetch(tex, uv + ivec2(1, 0), 0).r;
    o += texelFetch(tex, uv + ivec2(1, 1), 0).r;
#endif

    return o * 0.25;
}

void main()
{
    FragColor.r = Downsample4(RT, ivec2(gl_FragCoord.xy * 2.0));
}
