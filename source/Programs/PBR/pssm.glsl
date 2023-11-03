// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL

#if MAX_SHADOW_TEXTURES > 0
//#define PENUMBRA
uniform mediump sampler2D ShadowTex;
uniform vec4 ShadowDepthRangeArray[MAX_SHADOW_TEXTURES];
uniform float LightCastsShadowsArray[MAX_LIGHTS];
uniform highp vec4 PssmSplitPoints;
uniform vec4 ShadowColour;

#ifndef PSSM_FILTER_RADIUS
#define PSSM_FILTER_RADIUS 4
#define PSSM_FILTER_RADIUS0 1.0
#define PSSM_FILTER_RADIUS1 0.3
#define PSSM_FILTER_RADIUS2 0.2
#define PSSM_FILTER_RADIUS3 0.1
#endif
#ifndef PSSM_FILTER_SIZE
#define PSSM_FILTER_SIZE 8
#endif
#ifndef PENUMBRA_FILTER_RADIUS
#define PENUMBRA_FILTER_RADIUS 12.0
#endif
#ifndef PENUMBRA_FILTER_SIZE
#define PENUMBRA_FILTER_SIZE 8
#endif
#ifndef PENUMBRA_LIGHT_SIZE
#define PENUMBRA_LIGHT_SIZE 10.0
#endif

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

float AvgBlockersDepthToPenumbra(float depth, float avgBlockersDepth)
{
    float penumbra = PENUMBRA_LIGHT_SIZE * (depth - avgBlockersDepth) / avgBlockersDepth;
    return saturate(penumbra * penumbra);
}

float Penumbra(mediump sampler2D shadowTex, vec2 uv, const vec2 tsize, float phi, float depth)
{
    float avgBlockersDepth = 0.0;
    float blockersCount = 0.0;

    for (int i = 0; i < PENUMBRA_FILTER_SIZE; ++i) {
        vec2 offset = VogelDiskSample(i, PENUMBRA_FILTER_SIZE, phi) * tsize * float(PENUMBRA_FILTER_RADIUS);
        uv.xy += offset;
        float sampleDepth = texture2D(shadowTex, uv).x * 64.0 + 1.0;
        float depthTest = saturate(depth - sampleDepth);
        avgBlockersDepth += depthTest * sampleDepth;
        blockersCount += depthTest;
    }

    return AvgBlockersDepthToPenumbra(depth, avgBlockersDepth / blockersCount);
}

float CalcDepthShadow(mediump sampler2D shadowTex, vec4 uv)
{
    uv /= uv.w;
    uv.z = uv.z * 0.5 + 0.5;
    vec2 tsize = 0.5 / vec2(textureSize(shadowTex, 0));
    float shadow = 0.0;
    highp float currentDepth = uv.z - 4.0 * HALF_EPSILON;
    float phi = InterleavedGradientNoise();
#ifdef PENUMBRA
    float penumbra = Penumbra(shadowTex, uv.xy, tsize, phi, currentDepth);
#else
    const float penumbra = 1.0;
#endif

    for (int i = 0; i < PSSM_FILTER_SIZE; ++i) {
        vec2 offset = VogelDiskSample(i, PSSM_FILTER_SIZE, phi) * tsize * float(PSSM_FILTER_RADIUS) * penumbra;
        uv.xy += offset;
        highp float depth = texture2D(shadowTex, uv.xy).x * 64.0 + 1.0;
        shadow += fstep(depth, currentDepth);
    }

    shadow /= float(PSSM_FILTER_SIZE);

    return shadow;
}

float CalcPSSMShadow(const highp vec4 lightSpacePos0, const highp vec4 lightSpacePos1, const highp vec4 lightSpacePos2)
{
    highp float depth = gl_FragCoord.z / gl_FragCoord.w;
    if (depth <= PssmSplitPoints.x) return CalcDepthShadow(ShadowTex, vec4(lightSpacePos0.xy * 0.5, lightSpacePos0.zw));
    else if (depth <= PssmSplitPoints.y) return CalcDepthShadow(ShadowTex, vec4(lightSpacePos1.xy * 0.5 + vec2(0.5, 0.0), lightSpacePos1.zw));
    else if (depth <= PssmSplitPoints.z) return CalcDepthShadow(ShadowTex, vec4(lightSpacePos2.xy * 0.5 + vec2(0.0, 0.5), lightSpacePos2.zw));
    else return 1.0;
}

float CalcShadow(const vec4 lightSpacePos, int index)
{
    return CalcDepthShadow(ShadowTex, lightSpacePos);
}

#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
