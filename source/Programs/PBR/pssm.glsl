// created by Andrey Vasiliev

#ifndef PSSM_GLSL
#define PSSM_GLSL

#if MAX_SHADOW_TEXTURES > 0
uniform mediump sampler2D ShadowTex0;
#endif
#if MAX_SHADOW_TEXTURES > 1
uniform mediump sampler2D ShadowTex1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform mediump sampler2D ShadowTex2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform mediump sampler2D ShadowTex3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform mediump sampler2D ShadowTex4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform mediump sampler2D ShadowTex5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform mediump sampler2D ShadowTex6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform mediump sampler2D ShadowTex7;
#endif
uniform vec4 ShadowDepthRangeArray[MAX_SHADOW_TEXTURES];
uniform float LightCastsShadowsArray[MAX_LIGHTS];
uniform highp vec4 PssmSplitPoints;
uniform vec4 ShadowColour;

#if MAX_SHADOW_TEXTURES > 0
#ifndef PSSM_SPLIT_COUNT
#define PSSM_SPLIT_COUNT 1
#endif
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
#define PENUMBRA_FILTER_RADIUS 12
#endif
#ifndef PENUMBRA_FILTER_SIZE
#define PENUMBRA_FILTER_SIZE 12
#endif
#ifndef PENUMBRA_LIGHT_SIZE
#define PENUMBRA_LIGHT_SIZE 50.0
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
    return clamp(80.0 * penumbra * penumbra, 0.0, 1.0);
}

float Penumbra(mediump sampler2D shadowTex, vec2 uv, vec2 tsize, float phi, float depth)
{
    float avgBlockersDepth = 0.0;
    float blockersCount = 0.0;

    for (int i = 0; i < PENUMBRA_FILTER_SIZE; ++i) {
        vec2 offsetUV = VogelDiskSample(i, PENUMBRA_FILTER_SIZE, phi) * tsize * float(PENUMBRA_FILTER_RADIUS);
        float sampleDepth = texture2D(shadowTex, uv + offsetUV).x * 1000.0 + 1.0;
        float depthTest = clamp((depth - sampleDepth), 0.0, 1.0);
        avgBlockersDepth += depthTest * sampleDepth;
        blockersCount += depthTest;
    }

    return AvgBlockersDepthToPenumbra(depth, avgBlockersDepth / blockersCount);
}

float CalcDepthShadow(mediump sampler2D shadowTex, highp vec4 uv)
{
    uv /= uv.w;
    uv.z = uv.z * 0.5 + 0.5;
    vec2 tsize = 1.0 / vec2(textureSize(shadowTex, 0));
    float shadow = 0.0;
    highp float currentDepth = uv.z - 4.0 * HALF_EPSILON;
    float phi = InterleavedGradientNoise();
#ifdef PENUMBRA
    float penumbra = Penumbra(shadowTex, uv.xy, tsize, currentDepth);
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

#if PSSM_SPLIT_COUNT > 0
float CalcPSSMShadow(highp vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES]) {
    highp float depth = gl_FragCoord.z / gl_FragCoord.w;

    if (depth <= PssmSplitPoints.x) return CalcDepthShadow(ShadowTex0, lightSpacePosArray[0]);
#if PSSM_SPLIT_COUNT > 1
    else if (depth <= PssmSplitPoints.y) return CalcDepthShadow(ShadowTex1, lightSpacePosArray[1]);
#endif
#if PSSM_SPLIT_COUNT > 2
    else if (depth <= PssmSplitPoints.z) return CalcDepthShadow(ShadowTex2, lightSpacePosArray[2]);
#endif
#if PSSM_SPLIT_COUNT > 3
    else if (depth <= PssmSplitPoints.w) return CalcDepthShadow(ShadowTex3, lightSpacePosArray[3]);
#endif
    else return 1.0;
}
#endif


float CalcShadow(highp vec4 lightSpacePos, int index)
{
#if MAX_SHADOW_TEXTURES > 0
    if (index == 0) return CalcDepthShadow(ShadowTex0, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 1
    else if (index == 1) return CalcDepthShadow(ShadowTex1, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 2
    else if (index == 2) return CalcDepthShadow(ShadowTex2, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 3
    else if (index == 3) return CalcDepthShadow(ShadowTex3, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 4
    else if (index == 4) return CalcDepthShadow(ShadowTex4, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 5
    else if (index == 5) return CalcDepthShadow(ShadowTex5, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 6
    else if (index == 6) return CalcDepthShadow(ShadowTex6, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 7
    else if (index == 7) return CalcDepthShadow(ShadowTex7, lightSpacePos);
#endif
#if MAX_SHADOW_TEXTURES > 0
    else return 1.0;
#endif
}

#endif // MAX_SHADOW_TEXTURES > 0
#endif // PSSM_GLSL
