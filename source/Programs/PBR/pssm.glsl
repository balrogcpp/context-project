// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL

#ifdef TERRA_LIGHTMAP
uniform sampler2D TerraLightTex;
#endif

// https://drdesten.github.io/web/tools/vogel_disk/
const vec2 vogel_disk_4[4] = vec2[](
    vec2(0.21848650099008202, -0.09211370200809937),
    vec2(-0.5866112654782878, 0.32153793477769893),
    vec2(-0.06595078555407359, -0.879656059066481),
    vec2(0.43407555004227927, 0.6502318262968816)
);

const vec2 vogel_disk_8[8] = vec2[](
    vec2(0.2921473492144121, 0.03798942536906266),
    vec2(-0.27714274097351554, 0.3304853027892154),
    vec2(0.09101981507673855, -0.5188871157785563),
    vec2(0.44459182774878003, 0.5629069824170247),
    vec2(-0.6963877647721594, -0.09264703741542105),
    vec2(0.7417522811565185, -0.4070419658858473),
    vec2(-0.191856808948964, 0.9084732299066597),
    vec2(-0.40412395850181015, -0.8212788214021378)
);

const vec2 vogel_disk_16[16] = vec2[](
    vec2(0.18993645671348536, 0.027087114076591513),
    vec2(-0.21261242652069953, 0.23391293246949066),
    vec2(0.04771781344140756, -0.3666840644525993),
    vec2(0.297730981239584, 0.398259878229082),
    vec2(-0.509063425827436, -0.06528681462854097),
    vec2(0.507855152944665, -0.2875976005206389),
    vec2(-0.15230616564632418, 0.6426121151781916),
    vec2(-0.30240170651828074, -0.5805072900736001),
    vec2(0.6978019230005561, 0.2771173334141519),
    vec2(-0.6990963248129052, 0.3210960724922725),
    vec2(0.3565142601623699, -0.7066415061851589),
    vec2(0.266890002328106, 0.8360191043249159),
    vec2(-0.7515861305520581, -0.41609876195815027),
    vec2(0.9102937449894895, -0.17014527555321657),
    vec2(-0.5343471434373126, 0.8058593459499529),
    vec2(-0.1133270115046468, -0.9490025827627441)
);

#if MAX_SHADOW_TEXTURES > 0
uniform mediump sampler2D ShadowTex;
uniform vec4 ShadowDepthRangeArray[MAX_SHADOW_TEXTURES];
uniform float LightCastsShadowsArray[MAX_LIGHTS];
uniform highp vec4 PssmSplitPoints;
uniform vec4 ShadowColour;

#ifndef PSSM_FILTER_SIZE
#ifndef GL_ES
#define PSSM_FILTER_SIZE 8
#else
#define PSSM_FILTER_SIZE 4
#endif
#endif
#ifndef PSSM_FILTER_RADIUS
#define PSSM_FILTER_RADIUS 2.0
#endif
#ifndef PENUMBRA_LIGHT_SIZE
#define PENUMBRA_LIGHT_SIZE 1
#endif
//#define PENUMBRA

float InterleavedGradientNoise()
{
    const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy)));
}

vec2 VogelDiskSample(int sampleIndex, int samplesCount, float phi)
{
    float r = sqrt((float(sampleIndex) + 0.5) / float(samplesCount));
    float theta = float(sampleIndex) * 2.4 + phi;
    return vec2(r * cos(theta), r * sin(theta));
}

#ifdef PENUMBRA
float AvgBlockersDepthToPenumbra(float depth, float avgBlockersDepth)
{
    return float(PENUMBRA_LIGHT_SIZE) * (depth - avgBlockersDepth) / avgBlockersDepth;
}

float Penumbra(vec2 uv, const vec2 tsize, float phi, float depth)
{
    float avgBlockersDepth = 0.0;
    float blockersCount = 0.0;

    for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
        vec2 offset = VogelDiskSample(i, PSSM_FILTER_SIZE, phi) * tsize * PSSM_FILTER_RADIUS;
        float sampleDepth = texture2D(ShadowTex, uv + offset).x;
        float depthTest = fstep(sampleDepth, depth);
        avgBlockersDepth += depthTest * sampleDepth;
        blockersCount += depthTest;
    }
    if (blockersCount > 0.0)
        return AvgBlockersDepthToPenumbra(depth, avgBlockersDepth / blockersCount);
    else
        return 0.0;
}
#endif

float CalcShadow(int index)
{
    vec4 lightSpacePos = vLightSpacePosArray[index];
    lightSpacePos.xyz /= lightSpacePos.w;
    vec2 uv = lightSpacePos.xy;
    float depth = lightSpacePos.z;
    if (uv.x <= 0.0 || uv.x >= 1.0 || uv.y <= 0.0 || uv.y >= 1.0) return 1.0;

    if (index == 0)
        uv = uv * 0.5;
    else if (index == 1)
        uv = uv * 0.5 + vec2(0.5, 0.0);
    else if (index == 2)
        uv = uv * 0.5 + vec2(0.0, 0.5);
    else
        return 1.0;

    depth = depth * 0.5 + 0.5;
    vec2 tsize = 1.0 / vec2(textureSize(ShadowTex, 0));
    //depth -= 0.001;
    float shadow = 0.0;
    float phi = InterleavedGradientNoise();
#ifdef PENUMBRA
    float penumbra = Penumbra(uv, tsize, phi, depth);
#endif

    #define PSSM_ESM_K 13.0
    #define PSSM_ESM_MIN -0.2
    for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
#if PSSM_FILTER_SIZE == 16
        vec2 offset = vogel_disk_16[(i + int(phi)) % PSSM_FILTER_SIZE] * tsize * PSSM_FILTER_RADIUS;
#elif PSSM_FILTER_SIZE == 8
        vec2 offset = vogel_disk_8[(i + int(phi)) % PSSM_FILTER_SIZE] * tsize * PSSM_FILTER_RADIUS;
#elif PSSM_FILTER_SIZE == 4
        vec2 offset = vogel_disk_4[(i + int(phi)) % PSSM_FILTER_SIZE] * tsize * PSSM_FILTER_RADIUS;
#elif PSSM_FILTER_SIZE == 1
        const vec2 offset = vec2(0.0, 0.0);
#else
        vec2 offset = VogelDiskSample(i, PSSM_FILTER_SIZE, phi) * tsize * PSSM_FILTER_RADIUS;
#endif
        float texDepth = texture2D(ShadowTex, uv + offset).x;
        //if (texDepth >= 1.0 || texDepth <= -1.0) return 1.0;
#ifdef PSSM_ESM_SHADOWMAP
        float sampled = saturate(exp(max(PSSM_ESM_MIN, PSSM_ESM_K * (texDepth - depth))));
        sampled = (1.0 - (4.0 * (1.0 - sampled)));
        texDepth = texDepth * PSSM_GLOBAL_RANGE + PSSM_GLOBAL_MIN_DEPTH;
        shadow += max(sampled, fstep(texDepth, depth));
#else
        texDepth = texDepth * PSSM_GLOBAL_RANGE + PSSM_GLOBAL_MIN_DEPTH;
        shadow += step(texDepth, depth);
#endif
    }
    shadow /= float(PSSM_FILTER_SIZE);

    return shadow;
}

float CalcPSSMShadow()
{
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    if (depth >= PssmSplitPoints.w) return 1.0;
    
    if (depth <= PssmSplitPoints.x)
        return CalcShadow(0);
    else if (depth <= PssmSplitPoints.y)
        return CalcShadow(1);
    else if (depth <= PssmSplitPoints.z)
        return CalcShadow(2);
    else
        return 1.0;
}

#endif // MAX_SHADOW_TEXTURES > 0

#ifdef TERRA_LIGHTMAP
float FetchTerraShadow(const vec2 uv)
{
    float shadow = 0.0;
    float phi = InterleavedGradientNoise();
    vec2 tsize = 1.0 / vec2(textureSize(TerraLightTex, 0));

    for (int i = 0; i < 4; ++i) {
        vec2 offset = vogel_disk_4[(i + int(phi)) % 4] * 2.0 * tsize;
        shadow += texture2D(TerraLightTex, uv + offset).x;
    }
    shadow *= 0.25;

    return shadow;
}
#endif

#endif // PSSM_GLSL
