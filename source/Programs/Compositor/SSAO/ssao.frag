// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"


in vec2 vUV0;
in vec3 vRay;
uniform sampler2D DepthMap;
uniform sampler2D NormalMap;
uniform mediump mat4 ProjMatrix;
uniform mediump mat4 InvViewMatrix;
uniform mediump float FarClipDistance;


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 hash(const mediump vec3 a)
{
    mediump vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b. zyx);
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    #define MAX_RAND_SAMPLES 14

    mediump vec3 RAND_SAMPLES[MAX_RAND_SAMPLES];
    RAND_SAMPLES[0] = vec3(1.0, 0.0, 0.0);
    RAND_SAMPLES[1] = vec3(-1.0, 0.0, 0.0);
    RAND_SAMPLES[2] = vec3(0.0, 1.0, 0.0);
    RAND_SAMPLES[3] = vec3(0.0, -1.0, 0.0);
    RAND_SAMPLES[4] = vec3(0.0, 0.0, 1.0);
    RAND_SAMPLES[5] = vec3(0.0, 0.0, -1.0);
    RAND_SAMPLES[6] = normalize(vec3(1.0, 1.0, 1.0));
    RAND_SAMPLES[7] = normalize(vec3(-1.0, 1.0, 1.0));
    RAND_SAMPLES[8] = normalize(vec3(1.0, -1.0, 1.0));
    RAND_SAMPLES[9] = normalize(vec3(1.0, 1.0, -1.0));
    RAND_SAMPLES[10] = normalize(vec3(-1.0, -1.0, 1.0));
    RAND_SAMPLES[11] = normalize(vec3(-1.0, 1.0, -1.0));
    RAND_SAMPLES[12] = normalize(vec3(1.0, -1.0, -1.0));
    RAND_SAMPLES[13] = normalize(vec3(-1.0, -1.0, -1.0));


    // constant expression != const int :(
    #define NUM_BASE_SAMPLES MAX_RAND_SAMPLES

    // random normal lookup from a texture and expand to [-1..1]
    mediump float depth = texture2D(DepthMap, vUV0).x;

    // IN.ray will be distorted slightly due to interpolation
    // it should be normalized here
    mediump vec3 viewPos = vRay * depth;
    mediump vec3 worldPos = mul(InvViewMatrix, vec4(viewPos, 1.0)).xyz;
    mediump vec3 randN = hash(worldPos);

    // By computing Z manually, we lose some accuracy under extreme angles
    // considering this is just for bias, this loss is acceptable
    mediump vec3 viewNorm = texture2D(NormalMap, vUV0).xyz;

    // Accumulated occlusion factor
    mediump float occ = 0.0;

    for (int i = 0; i < NUM_BASE_SAMPLES; ++i) {
        // Reflected direction to move in for the sphere
        // (based on random samples and a random texture sample)
        // bias the random direction away from the normal
        // this tends to minimize self occlusion
        mediump vec3 randomDir = reflect(RAND_SAMPLES[i], randN) + viewNorm;

        // Move new view-space position back into texture space
        //#define RADIUS 0.2125
        #define RADIUS 0.0525

        mediump vec4 nuv = mul(ProjMatrix, vec4(viewPos + randomDir * RADIUS, 1.0));
        nuv /= nuv.w;

        // Compute occlusion based on the (scaled) Z difference
        mediump float zd = clamp(FarClipDistance * (depth - texture2D(DepthMap, nuv.xy).x), 0.0, 1.0);
        // This is a sample occlusion function, you can always play with
        // other ones, like 1.0 / (1.0 + zd * zd) and stuff
        occ += clamp(pow(1.0 - zd, 11.0) + zd, 0.0, 1.0);
    }

    // normalise
    occ /= float(NUM_BASE_SAMPLES);

    // amplify and saturate if necessary
    FragColor = vec4(occ, 0.0, 0.0, 1.0);
}
