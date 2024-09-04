// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy * 2.0);

    float c = texelFetch(RT, uv, 0).r;
    c += texelFetch(RT, uv + ivec2(0, 1), 0).r;
    c += texelFetch(RT, uv + ivec2(1, 0), 0).r;
    c += texelFetch(RT, uv + ivec2(1, 1), 0).r;

    float lum = c * (1.0 / 4.0);

    FragColor.r = lum;
}
