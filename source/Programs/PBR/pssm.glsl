// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL
#if MAX_SHADOW_TEXTURES > 0

uniform mediump sampler2D ShadowTex;
uniform vec4 ShadowDepthRangeArray[MAX_SHADOW_TEXTURES];
uniform float LightCastsShadowsArray[MAX_LIGHTS];
uniform highp vec4 PssmSplitPoints;
uniform vec4 ShadowColour;

#ifndef PSSM_FILTER_SIZE
#ifndef GL_ES
#define PSSM_FILTER_SIZE 6
#else
#define PSSM_FILTER_SIZE 4
#endif
#endif
#ifndef PSSM_FILTER_RADIUS
#define PSSM_FILTER_RADIUS 2
#endif
#ifndef PENUMBRA_LIGHT_SIZE
#define PENUMBRA_LIGHT_SIZE 1
#endif
//#define PENUMBRA

float InterleavedGradientNoise()
{
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy)));
}

vec2 VogelDiskSample(int sampleIndex, int samplesCount, float phi)
{
    float r = sqrt((float(sampleIndex) + 0.5) / float(samplesCount));
    float theta = float(sampleIndex) * 2.4 + phi;
    return vec2(r * cos(theta), r * sin(theta));
}

float AvgBlockersDepthToPenumbra(float depth, float avgBlockersDepth)
{
    float penumbra = float(PENUMBRA_LIGHT_SIZE) * (depth - avgBlockersDepth) / avgBlockersDepth;
    return penumbra;
}

float Penumbra(vec2 uv, const vec2 tsize, float phi, float depth)
{
    float avgBlockersDepth = 0.0;
    float blockersCount = 0.0;

    for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
        vec2 offset = VogelDiskSample(i, PSSM_FILTER_SIZE, phi) * tsize * float(PSSM_FILTER_RADIUS);
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

float CalcDepthShadow(vec2 uv, float currentDepth, const vec2 tsize)
{
    currentDepth = currentDepth * 0.5 + 0.5;
    float shadow = 0.0;
    float phi = InterleavedGradientNoise();
#ifdef PENUMBRA
    float penumbra = Penumbra(uv, tsize, phi, currentDepth);
#else
    const float penumbra = 1.0;
#endif

    for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
        vec2 offset = VogelDiskSample(i, PSSM_FILTER_SIZE, phi) * tsize * float(PSSM_FILTER_RADIUS) * penumbra;
        float texDepth = texture2D(ShadowTex, uv + offset).x;
#ifdef PSSM_ESM_SHADOWMAP
        float sampled = saturate(texDepth * exp(-PSSM_ESM_K * currentDepth));
        sampled = 1.0 - (PSSM_ESM_SCALE * (1.0 - sampled));
#else
        float sampled = fstep(texDepth, currentDepth);
#endif
        shadow += sampled;
    }
    shadow /= float(PSSM_FILTER_SIZE);

    return shadow;
}

float CalcPSSMShadow(const highp vec4 lightSpacePos0, const highp vec4 lightSpacePos1, const highp vec4 lightSpacePos2)
{
    highp float depth = gl_FragCoord.z / gl_FragCoord.w;
    vec2 tsize = 1.0 / vec2(textureSize(ShadowTex, 0));

    if (depth <= PssmSplitPoints.x) return CalcDepthShadow(saturate(lightSpacePos0.xy / lightSpacePos0.w) * 0.5, (lightSpacePos0.z / lightSpacePos0.w - ShadowDepthRangeArray[0].x) * ShadowDepthRangeArray[0].w, tsize);
    else if (depth <= PssmSplitPoints.y) return CalcDepthShadow(saturate(lightSpacePos1.xy / lightSpacePos1.w) * 0.5 + vec2(0.5, 0.0), (lightSpacePos1.z / lightSpacePos1.w - ShadowDepthRangeArray[1].x) * ShadowDepthRangeArray[1].w, tsize);
    else if (depth <= PssmSplitPoints.z) return CalcDepthShadow(saturate(lightSpacePos2.xy / lightSpacePos2.w) * 0.5 + vec2(0.0, 0.5), (lightSpacePos2.z / lightSpacePos2.w - ShadowDepthRangeArray[2].x) * ShadowDepthRangeArray[2].w, tsize);
    else return 1.0;
}

float CalcShadow(const vec4 lightSpacePos, int index)
{
    vec2 tsize = 1.0 / vec2(textureSize(ShadowTex, 0));
    return CalcDepthShadow(lightSpacePos.xy / lightSpacePos.w, lightSpacePos.z / lightSpacePos.w, tsize);
}

#ifdef TERRA_NORMALMAP
float FetchTerraShadow(sampler2D shadowTex, vec2 uv)
{
    float shadow = 0.0;
    float phi = InterleavedGradientNoise();
    vec2 tsize = 1.0 / vec2(textureSize(shadowTex, 0));

    for (int i = 0; i < 4; ++i) {
        vec2 offset = VogelDiskSample(i, 4, phi) * 2.0 * tsize;
        uv += offset;
        shadow += texture2D(shadowTex, uv).x;
    }

    shadow *= 0.25;

    return shadow;
}
#endif

#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
