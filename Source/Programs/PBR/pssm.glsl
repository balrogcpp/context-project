// created by Andrey Vasiliev
//? #version 400

#define PI                 3.14159265359
#define PSSM_FILTER_SIZE   4
#define PSSM_FILTER_RADIUS 2.0
#define PSSM_ESM_SHADOWMAP
#define PSSM_ESM_K 13.0
#define PSSM_ESM_MIN -0.2


// https://drdesten.github.io/web/tools/vogel_disk/
mediump vec2 vogel_disk_4[4] = vec2[](
    vec2(0.21848650099008202, -0.09211370200809937),
    vec2(-0.5866112654782878, 0.32153793477769893),
    vec2(-0.06595078555407359, -0.879656059066481),
    vec2(0.43407555004227927, 0.6502318262968816)
);

mediump vec2 vogel_disk_8[8] = vec2[](
    vec2(0.2921473492144121, 0.03798942536906266),
    vec2(-0.27714274097351554, 0.3304853027892154),
    vec2(0.09101981507673855, -0.5188871157785563),
    vec2(0.44459182774878003, 0.5629069824170247),
    vec2(-0.6963877647721594, -0.09264703741542105),
    vec2(0.7417522811565185, -0.4070419658858473),
    vec2(-0.191856808948964, 0.9084732299066597),
    vec2(-0.40412395850181015, -0.8212788214021378)
);

float InterleavedGradientNoise(const vec2 uv)
{
    const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

mat2 getRandomRotationMatrix(const vec2 fragCoord) {
    // rotate the poisson disk randomly
    float randomAngle = InterleavedGradientNoise(fragCoord) * (2.0 * PI);
    vec2 randomBase = vec2(cos(randomAngle), sin(randomAngle));
    mat2 R = mat2(randomBase.x, randomBase.y, -randomBase.y, randomBase.x);
    return R;
}

#ifdef TERRA_LIGHTMAP
float FetchTerraShadow(const vec2 uv)
{
    float shadow = 0.0;
    mat2 R = getRandomRotationMatrix(gl_FragCoord.xy);

    for (int i = 0; i < 4; ++i) {
        vec2 offset = R * (vogel_disk_4[i] * 2.0);
        shadow += textureLod(TerraLightTex, uv + offset, 0.0).x;
    }
    shadow *= 0.25;

    return shadow;
}
#endif


#if MAX_SHADOW_TEXTURES > 0
float CalcShadow(const highp vec3 lightSpacePos, const int index)
{
    vec2 uv = lightSpacePos.xy;
    float depth = lightSpacePos.z;

    depth = depth * 0.5 + 0.5;
    float shadow = 0.0;
    mat2 R = getRandomRotationMatrix(gl_FragCoord.xy);

    for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
        vec2 offset = (R * vogel_disk_4[i]) * PSSM_FILTER_RADIUS;

        float texDepth = 0.0;
        if (index == 0)
            texDepth = textureLod(ShadowTex0, uv + offset * TexelSize6, 0.0).x;
        else if (index == 1)
            texDepth = textureLod(ShadowTex1, uv + offset * TexelSize7, 0.0).x;
        else if (index == 2)
            texDepth = textureLod(ShadowTex2, uv + offset * TexelSize8, 0.0).x;
        else if (index == 3)
            texDepth = textureLod(ShadowTex3, uv + offset * TexelSize9, 0.0).x;

        texDepth = texDepth * 64.0 + 1.0;
        float sampled = saturate(exp(max(PSSM_ESM_MIN, PSSM_ESM_K * (texDepth - depth))));
        sampled = (1.0 - (4.0 * (1.0 - sampled)));
        shadow += sampled;
    }

    shadow /= float(PSSM_FILTER_SIZE);

    return shadow;
}
#endif
