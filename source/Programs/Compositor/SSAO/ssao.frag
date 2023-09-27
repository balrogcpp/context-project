// created by Andrey Vasiliev
// based on https://github.com/OGRECave/ogre/blob/v13.6.4/Samples/Media/DeferredShadingMedia/ssao_ps.glsl

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"

uniform sampler2D DepthMap;
uniform sampler2D NormalMap;

uniform mediump mat4 ProjMatrix;
uniform mediump float FarClipDistance;
uniform mediump float NearClipDistance;

mediump vec3 hash(const mediump vec3 a)
{
    mediump vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b.zyx);
}

mediump float ClampDepth(const mediump float linearDepth)
{
    return (linearDepth - NearClipDistance) / (FarClipDistance - NearClipDistance);
}

mediump float LineariseDepth(const mediump float clampedDepth)
{
    return clampedDepth * (FarClipDistance - NearClipDistance) + NearClipDistance;
}

mediump vec3 GetScreenSpacePos(const mediump vec2 uv, const mediump vec3 ray)
{
    mediump float clampedDepth = texture2D(DepthMap, uv).x;
    mediump float linearDepth = LineariseDepth(clampedDepth);
    return ray * linearDepth;
}

in mediump vec2 vUV0;
in highp vec3 vRay;
void main()
{
    #define MAX_RAND_SAMPLES 14
    #define RADIUS 0.105

    const mediump vec3 RAND_SAMPLES[MAX_RAND_SAMPLES] =
        vec3[](
        vec3(1.0, 0.0, 0.0),
        vec3(-1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, -1.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        vec3(0.0, 0.0, -1.0),
        normalize(vec3(1.0, 1.0, 1.0)),
        normalize(vec3(-1.0, 1.0, 1.0)),
        normalize(vec3(1.0, -1.0, 1.0)),
        normalize(vec3(1.0, 1.0, -1.0)),
        normalize(vec3(-1.0, -1.0, 1.0)),
        normalize(vec3(-1.0, 1.0, -1.0)),
        normalize(vec3(1.0, -1.0, -1.0)),
        normalize(vec3(-1.0, -1.0, -1.0)));

    // constant expression != const int :(
#ifndef GL_ES
    #define NUM_BASE_SAMPLES MAX_RAND_SAMPLES
#else
    #define NUM_BASE_SAMPLES 6
#endif

    // random normal lookup from a texture and expand to [-1..1]
    // IN.ray will be distorted slightly due to interpolation
    // it should be normalized here
    mediump float clampedDepth = texture2D(DepthMap, vUV0).x;
    mediump vec3 viewPos = vRay * clampedDepth;
    mediump vec3 randN = hash(gl_FragCoord.xyz) * pow5(1.0 - clampedDepth);

    // By computing Z manually, we lose some accuracy under extreme angles
    // considering this is just for bias, this loss is acceptable
    mediump vec3 viewNorm = texture2D(NormalMap, vUV0).xyz * 2.0 - 1.0;

    vec2 p = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y));
    if(clampedDepth > 0.5 || (mod(p.y, 2.0) == 0.0 && mod(p.x, 2.0) == 0.0)) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }

    // Accumulated occlusion factor
    mediump float occ = 0.0;
    for (int i = 0; i < NUM_BASE_SAMPLES; ++i) {
        // Reflected direction to move in for the sphere
        // (based on random samples and a random texture sample)
        // bias the random direction away from the normal
        // this tends to minimize self occlusion
        mediump vec3 randomDir = reflect(RAND_SAMPLES[i], randN) + viewNorm.xyz;

        mediump vec3 oSample = viewPos + randomDir * RADIUS;
        mediump vec4 nuv = mul(ProjMatrix, vec4(oSample, 1.0));
        nuv.xy /= nuv.w;

        // Compute occlusion based on the (scaled) Z difference
        mediump float sampleDepth = texture2D(DepthMap, nuv.xy).x;
        mediump float rangeCheck = smoothstep(0.0, 1.0, RADIUS / (FarClipDistance * (clampedDepth - sampleDepth)) ) * bigger(sampleDepth, oSample.z);

        // This is a sample occlusion function, you can always play with
        // other ones, like 1.0 / (1.0 + zd * zd) and stuff
        occ += clamp(pow10(1.0 - rangeCheck) + rangeCheck + 0.6667 * sqrt(clampedDepth), 0.0, 1.0);
    }

    // normalise
    occ /= float(NUM_BASE_SAMPLES);

    // amplify and saturate if necessary
    FragColor = vec4(occ, 0.0, 0.0, 1.0);
}
