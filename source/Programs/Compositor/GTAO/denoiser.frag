// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/asylum2010/Asylum_Tutorials/blob/4f2bc39a8ae69db1ceb59e9a763ef91c7b3dc6de/Media/ShadersGL/gtaospatialdenoiser.frag
void main()
{
    // NOTE: 4x4 filter offsets image
    ivec2 loc = ivec2(gl_FragCoord.xy) - ivec2(2, 2);
    float totalao = 0.0;

    // NOTE: textureGather requires GL 4
    totalao += texelFetch(RT, loc, 0).r;
    totalao += texelFetch(RT, loc + ivec2(1, 0), 0).r;
    totalao += texelFetch(RT, loc + ivec2(0, 1), 0).r;
    totalao += texelFetch(RT, loc + ivec2(1, 1), 0).r;

    totalao += texelFetch(RT, loc + ivec2(2, 0), 0).r;
    totalao += texelFetch(RT, loc + ivec2(3, 0), 0).r;
    totalao += texelFetch(RT, loc + ivec2(2, 1), 0).r;
    totalao += texelFetch(RT, loc + ivec2(3, 1), 0).r;

    totalao += texelFetch(RT, loc + ivec2(0, 2), 0).r;
    totalao += texelFetch(RT, loc + ivec2(1, 2), 0).r;
    totalao += texelFetch(RT, loc + ivec2(0, 3), 0).r;
    totalao += texelFetch(RT, loc + ivec2(1, 3), 0).r;

    totalao += texelFetch(RT, loc + ivec2(2, 2), 0).r;
    totalao += texelFetch(RT, loc + ivec2(3, 2), 0).r;
    totalao += texelFetch(RT, loc + ivec2(2, 3), 0).r;
    totalao += texelFetch(RT, loc + ivec2(3, 3), 0).r;

    FragColor.r = totalao * 0.0625;
}
