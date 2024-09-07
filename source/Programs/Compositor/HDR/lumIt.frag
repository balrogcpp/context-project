// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

float Downsample4(const sampler2D tex, const ivec2 uv)
{
#ifndef GL_ES
    vec4 g = textureGather(tex, uv / textureSize(tex, 0).xy, 0);
    float c = g.x + g.y + g.z + g.w;
#else
    float c = texelFetch(tex, uv, 0).r;
    c += texelFetch(tex, uv + ivec2(0, 1), 0).r;
    c += texelFetch(tex, uv + ivec2(1, 0), 0).r;
    c += texelFetch(tex, uv + ivec2(1, 1), 0).r;
#endif

    return c * 0.25;
}

void main()
{
    float lum = Downsample4(RT, ivec2(gl_FragCoord.xy * 2.0));

    FragColor.r = lum;
}
