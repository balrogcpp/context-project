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

SAMPLER2D(DepthMap, 0);
SAMPLER2D(NormalMap, 1);
uniform mediump mat4 ProjMatrix;
uniform mediump mat4 ViewMatrix;
uniform mediump mat4 InvProjMatrix;
uniform mediump float FarClipDistance;


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 hash(const mediump vec3 a)
{
    mediump vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b.zyx);
}


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)
IN(highp vec3 vRay, TEXCOORD1)

MAIN_DECLARATION
{
    #define MAX_RAND_SAMPLES 14

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
    #define NUM_BASE_SAMPLES 6

    // random normal lookup from a texture and expand to [-1..1]
    mediump float depth = texture2D(DepthMap, vUV0).r;

    // IN.ray will be distorted slightly due to interpolation
    // it should be normalized here
    mediump vec3 viewPos = vRay * depth;
    mediump vec3 worldPos = mul(InvProjMatrix, vec4(viewPos, 1.0)).xyz;
    mediump vec3 randN = hash(worldPos);

    // By computing Z manually, we lose some accuracy under extreme angles
    // considering this is just for bias, this loss is acceptable
    //mediump vec3 viewNorm = texture2D(NormalMap, vUV0).xyz;
    mediump vec4 viewNorm = mul(ViewMatrix, vec4(texture2D(NormalMap, vUV0).xyz, 0.0));

    // Accumulated occlusion factor
    mediump float occ = 0.0;

    for (mediump int i = 0; i < NUM_BASE_SAMPLES; ++i) {
        // Reflected direction to move in for the sphere
        // (based on random samples and a random texture sample)
        // bias the random direction away from the normal
        // this tends to minimize self occlusion
        mediump vec3 randomDir = reflect(RAND_SAMPLES[i], randN) + viewNorm.xyz;

        // Move new view-space position back into texture space
        //#define RADIUS 0.2125
        #define RADIUS 0.0525

        mediump vec4 nuv = mul(ProjMatrix, vec4(viewPos + randomDir * RADIUS, 1.0));
        nuv /= nuv.w;

        // Compute occlusion based on the (scaled) Z difference
        mediump float zd = clamp(FarClipDistance * (depth - texture2D(DepthMap, nuv.xy).x - 2.0 * HALF_EPSILON), 0.0, 1.0);
        // This is a sample occlusion function, you can always play with
        // other ones, like 1.0 / (1.0 + zd * zd) and stuff
        occ += clamp(pow(1.0 - zd, 11.0) + zd, 0.0, 1.0);
    }

    // normalise
    occ /= float(NUM_BASE_SAMPLES);

    // amplify and saturate if necessary
    FragColor = vec4(occ, 0.0, 0.0, 1.0);
}
