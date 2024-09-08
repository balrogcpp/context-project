// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/asylum2010/Asylum_Tutorials/blob/4f2bc39a8ae69db1ceb59e9a763ef91c7b3dc6de/Media/ShadersGL/gtaospatialdenoiser.frag
void main()
{
    ivec2 loc = ivec2(gl_FragCoord.xy) - ivec2(2, 2);
    float totalao = 0.0;

    // NOTE: 4x4 filter offsets image
    // NOTE: textureGather requires GL 4

#if !defined(GL_ES)
    vec2 tsize = 1.0 / vec2(textureSize(RT, 0));
    vec2 uv = loc / vec2(textureSize(RT, 0));

    vec4 g1 = textureGather(RT, uv, 0);
    vec4 g2 = textureGather(RT, uv + tsize * vec2(2.0, 0.0), 0);
    vec4 g3 = textureGather(RT, uv + tsize * vec2(0.0, 2.0), 0);
    vec4 g4 = textureGather(RT, uv + tsize * vec2(2.0, 2.0), 0);

    totalao += g1.x + g1.y + g1.z + g1.w;
    totalao += g2.x + g2.y + g2.z + g2.w;
    totalao += g3.x + g3.y + g3.z + g3.w;
    totalao += g4.x + g4.y + g4.z + g4.w;

#else
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
