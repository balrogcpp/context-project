// created by Andrey Vasiliev
// based on https://github.com/OGRECave/ogre/blob/v13.6.4/Samples/Media/DeferredShadingMedia/ssao_ps.glsl

#include "header.glsl"

uniform sampler2D DepthTex;
uniform sampler2D NormalTex;
uniform mat4 ProjMatrix;
uniform float FarClipDistance;
uniform float NearClipDistance;

vec3 hash(const vec3 a)
{
    vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b.zyx);
}

float pow5(float x)
{
    float x2 = x * x;
    return x2 * x2 * x;
}

float pow8(float x)
{
    float x2 = x * x;
    float x4 = x2 * x2;
    return x4 * x4;
}

float pow10(float x)
{
    float x2 = x * x;
    float x4 = x2 * x2;
    return x4 * x4 * x2;
}


in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    #define MAX_RAND_SAMPLES 14
    #define RADIUS 0.105
    #define INVSQ3 0.57735026918962576451
#ifndef GL_ES
    #define NUM_BASE_SAMPLES MAX_RAND_SAMPLES
#else
    #define NUM_BASE_SAMPLES 6
#endif

    const vec3 RAND_SAMPLES[MAX_RAND_SAMPLES] =
        vec3[](
        vec3(1.0, 0.0, 0.0),
        vec3(-1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, -1.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        vec3(0.0, 0.0, -1.0),
        vec3( INVSQ3,  INVSQ3,  INVSQ3),
        vec3(-INVSQ3,  INVSQ3,  INVSQ3),
        vec3( INVSQ3, -INVSQ3,  INVSQ3),
        vec3( INVSQ3,  INVSQ3, -INVSQ3),
        vec3(-INVSQ3, -INVSQ3,  INVSQ3),
        vec3(-INVSQ3,  INVSQ3, -INVSQ3),
        vec3( INVSQ3, -INVSQ3, -INVSQ3),
        vec3(-INVSQ3, -INVSQ3, -INVSQ3)
    );

    // random normal lookup from a texture and expand to [-1..1]
    // IN.ray will be distorted slightly due to interpolation
    // it should be normalized here
    float clampedPixelDepth = texture2D(NormalTex, vUV0).x;
    float pixelDepth = clampedPixelDepth * (FarClipDistance - NearClipDistance);
    vec3 viewPos = vRay * clampedPixelDepth;
    float invDepth = 1.0 - clampedPixelDepth;
    vec3 randN = hash(gl_FragCoord.xyz) * invDepth * invDepth;

    // By computing Z manually, we lose some accuracy under extreme angles
    // considering this is just for bias, this loss is acceptable
    vec3 normal = unpack(texture2D(NormalTex, vUV0).y) * 2.0 - 1.0;

    if(normal == vec3(0.0, 0.0, 0.0) || clampedPixelDepth > 0.5) {
        FragColor.r = 1.0;
        return;
    }

    // Accumulated occlusion factor
    float occ = 0.0;
    for (int i = 0; i < NUM_BASE_SAMPLES; ++i) {
        // Reflected direction to move in for the sphere
        // (based on random samples and a random texture sample)
        // bias the random direction away from the normal
        // this tends to minimize self occlusion
        vec3 randomDir = reflect(RAND_SAMPLES[i], randN) + normal;

        vec3 oSample = viewPos + randomDir * RADIUS;
        vec4 nuv = mul(ProjMatrix, vec4(oSample, 1.0));
        nuv.xy /= nuv.w;

        // Compute occlusion based on the (scaled) Z difference
        float clampedSampleDepth = texture2D(DepthTex, nuv.xy).x;
        float sampleDepth = clampedSampleDepth * (FarClipDistance - NearClipDistance);
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / (pixelDepth - sampleDepth)) * fstep(clampedSampleDepth, oSample.z);

        // This is a sample occlusion function, you can always play with
        // other ones, like 1.0 / (1.0 + zd * zd) and stuff
        occ += clamp(pow10(1.0 - rangeCheck) + rangeCheck + 0.6666667 * sqrt(clampedPixelDepth), 0.0, 1.0);
    }

    // normalise
    occ /= float(NUM_BASE_SAMPLES);
    occ = sqrt(occ * occ * occ);

    // amplify and saturate if necessary
    FragColor.rg = vec2(occ, clampedPixelDepth);
}
