// created by Andrey Vasiliev
//? #version 400

uniform sampler2D RT;
uniform sampler2D Depth;

#define SafeHDR(x) min(x, 65504.0)

vec2 pack(const mediump float depth) {
    // this is equivalent to (x8 * 256 + y8) / 65535, which gives a value between 0 and 1
  const float toFixed = 256.0/257.0;
  return vec2((depth * toFixed * 1.0), (depth * toFixed * 257.0));
}


// https://github.com/asylum2010/Asylum_Tutorials/blob/4f2bc39a8ae69db1ceb59e9a763ef91c7b3dc6de/Media/ShadersGL/gtaospatialdenoiser.frag
out vec2 FragColor;
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
    ivec2 uv = ivec2(gl_FragCoord.xy) - ivec2(2, 2);

    totalao += texelFetchOffset(RT, uv, 0, ivec2(0, 0)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(1, 0)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(0, 1)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(1, 1)).r;

    totalao += texelFetchOffset(RT, uv, 0, ivec2(2, 0)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(3, 0)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(2, 1)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(3, 1)).r;

    totalao += texelFetchOffset(RT, uv, 0, ivec2(0, 2)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(1, 2)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(0, 3)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(1, 3)).r;

    totalao += texelFetchOffset(RT, uv, 0, ivec2(2, 2)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(3, 2)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(2, 3)).r;
    totalao += texelFetchOffset(RT, uv, 0, ivec2(3, 3)).r;
#endif

    float d = texelFetch(Depth, ivec2(gl_FragCoord.xy) - ivec2(2, 2), 0).x;
    FragColor = vec2(SafeHDR(totalao * 0.0625), (d));
}
