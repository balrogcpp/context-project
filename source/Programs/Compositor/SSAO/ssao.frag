// created by Andrey Vasiliev
// based on https://github.com/OGRECave/ogre/blob/v13.6.4/Samples/Media/DeferredShadingMedia/ssao_ps.glsl

#include "header.glsl"
#include "mosaic.glsl"

uniform sampler2D DepthTex;
uniform sampler2D NormalTex;
uniform mat4 ProjMatrix;
uniform float ClipDistance;

vec3 hash(vec3 a)
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

float pow10(float x)
{
    float x2 = x * x;
    float x4 = x2 * x2;
    return x4 * x4 * x2;
}

in vec2 vUV0;
in vec3 vRay;
void main()
{
    #define MAX_RAND_SAMPLES 14
    #define RADIUS 0.105

    const vec3 RAND_SAMPLES[MAX_RAND_SAMPLES] =
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
    float clampedPixelDepth = texture2D(DepthTex, vUV0).x;
    float pixelDepth = clampedPixelDepth * ClipDistance;
    vec3 viewPos = vRay * clampedPixelDepth;
    vec3 randN = hash(gl_FragCoord.xyz) * pow5(1.0 - clampedPixelDepth);

    // By computing Z manually, we lose some accuracy under extreme angles
    // considering this is just for bias, this loss is acceptable
    vec3 normal = texture2D(NormalTex, vUV0).xyz;

    if(clampedPixelDepth > 0.5 || clampedPixelDepth < HALF_EPSILON || Null(normal) || ExcludePixel()) {
        FragColor.r = 1.0;
        return;
    }

    // Accumulated occlusion factor
    float occlusion = 0.0;
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
        float sampleDepth = clampedSampleDepth * ClipDistance;
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / (pixelDepth - sampleDepth)) * fstep(clampedSampleDepth, oSample.z);

        // This is a sample occlusion function, you can always play with
        // other ones, like 1.0 / (1.0 + zd * zd) and stuff
        occlusion += clamp(pow10(1.0 - rangeCheck) + rangeCheck + 0.6666667 * sqrt(clampedPixelDepth), 0.0, 1.0);
    }

    // normalise
    occlusion /= float(NUM_BASE_SAMPLES);

    // amplify and saturate if necessary
    FragColor.r = occlusion;
}
