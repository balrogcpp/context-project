// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/asylum2010/Asylum_Tutorials/blob/4f2bc39a8ae69db1ceb59e9a763ef91c7b3dc6de/Media/ShadersGL/gtaospatialdenoiser.frag
void main()
{
    float totalao = 0.0;

    // NOTE: 4x4 filter offsets image
    // NOTE: textureGather requires GL 4

#if __VERSION__ > 330
    vec2 uv = vec2(gl_FragCoord.xy - vec2(1.5, 1.5)) / vec2(textureSize(RT, 0));

    vec4 g1 = textureGatherOffset(RT, uv, ivec2(0, 0), 0);
    vec4 g2 = textureGatherOffset(RT, uv, ivec2(2, 0), 0);
    vec4 g3 = textureGatherOffset(RT, uv, ivec2(0, 2), 0);
    vec4 g4 = textureGatherOffset(RT, uv, ivec2(2, 2), 0);

    totalao += dot(g1, vec4(1.0, 1.0, 1.0, 1.0));
    totalao += dot(g2, vec4(1.0, 1.0, 1.0, 1.0));
    totalao += dot(g3, vec4(1.0, 1.0, 1.0, 1.0));
    totalao += dot(g4, vec4(1.0, 1.0, 1.0, 1.0));

#else
    ivec2 loc = ivec2(gl_FragCoord.xy) - ivec2(2, 2);

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
#endif

    FragColor.r = totalao * 0.0625;
}
