// created by Andrey Vasiliev
// based on https://github.com/OGRECave/ogre/blob/v13.6.4/Samples/Media/DeferredShadingMedia/ssao_ps.glsl

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "filters_F16.glsl"


varying mediump vec2 vUV0;
varying mediump vec3 vRay;
uniform sampler2D DepthMap;
uniform sampler2D NormalMap;
uniform vec2 TexelSize0;
uniform mediump mat4 ProjMatrix;
uniform mediump mat4 InvViewMatrix;
uniform mediump float FarClipDistance;


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 hash(const mediump vec3 a)
{
    mediump vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b.zyx);
}


//----------------------------------------------------------------------------------------------------------------------
mediump float iter(const mediump vec3 dir, const mediump vec3 viewNorm, const mediump vec3 viewPos, const mediump vec3 randN, const mediump float depth)
{
    // Reflected direction to move in for the sphere
    // (based on random samples and a random texture sample)
    // bias the random direction away from the normal
    // this tends to minimize self occlusion

   mediump vec3 randomDir = reflect(dir, randN) + viewNorm;

    // Move new view-space position back into texture space
    // #define RADIUS 0.2125
    #define RADIUS 0.0525

    mediump vec4 nuv = mul(ProjMatrix, vec4(viewPos + randomDir * RADIUS, 1.0));
    nuv /= nuv.w;

    // Compute occlusion based on the (scaled) Z difference
//    mediump float zd = clamp(FarClipDistance * (depth - texture2D(DepthMap, nuv.xy).x - HALF_EPSILON), 0.0, 1.0);
    mediump float zd = clamp(FarClipDistance * (depth - Downscale2x2_05(DepthMap, nuv.xy, TexelSize0) - HALF_EPSILON), 0.0, 1.0);
    // This is a sample occlusion function, you can always play with
    // other ones, like 1.0 / (1.0 + zd * zd) and stuff
    return clamp(pow(1.0 - zd, 11.0) + zd, 0.0, 1.0);
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    // random normal lookup from a texture and expand to [-1..1]
    mediump float depth = Downscale2x2_05(DepthMap, vUV0, TexelSize0);

    // IN.ray will be distorted slightly due to interpolation
    // it should be normalized here
    mediump vec3 viewPos = vRay * depth;
    mediump vec3 worldPos = mul(InvViewMatrix, vec4(viewPos, 1.0)).xyz;
    mediump vec3 randN = hash(worldPos);

    // By computing Z manually, we lose some accuracy under extreme angles
    // considering this is just for bias, this loss is acceptable
    mediump vec3 viewNorm = texture2D(NormalMap, vUV0).xyz;

    mediump float A = iter(vec3(1.0, 0.0, 0.0), viewNorm, viewPos, randN, depth);
    mediump float B = iter(vec3(-1.0, 0.0, 0.0), viewNorm, viewPos, randN, depth);
    mediump float C = iter(vec3(0.0, 1.0, 0.0), viewNorm, viewPos, randN, depth);
    mediump float D = iter(vec3(0.0, -1.0, 0.0), viewNorm, viewPos, randN, depth);
    mediump float E = iter(vec3(0.0, 0.0, 1.0), viewNorm, viewPos, randN, depth);
    mediump float F = iter(vec3(0.0, 0.0, -1.0), viewNorm, viewPos, randN, depth);
    mediump float G = iter(normalize(vec3(1.0, 1.0, 1.0)), viewNorm, viewPos, randN, depth);
    mediump float H = iter(normalize(vec3(-1.0, 1.0, 1.0)), viewNorm, viewPos, randN, depth);
    mediump float I = iter(normalize(vec3(1.0, -1.0, 1.0)), viewNorm, viewPos, randN, depth);
    mediump float J = iter(normalize(vec3(1.0, 1.0, -1.0)), viewNorm, viewPos, randN, depth);
    mediump float K = iter(normalize(vec3(-1.0, -1.0, 1.0)), viewNorm, viewPos, randN, depth);
    mediump float L = iter(normalize(vec3(-1.0, 1.0, -1.0)), viewNorm, viewPos, randN, depth);
    mediump float M = iter(normalize(vec3(1.0, -1.0, -1.0)), viewNorm, viewPos, randN, depth);
    mediump float N = iter(normalize(vec3(-1.0, -1.0, -1.0)), viewNorm, viewPos, randN, depth);

    // Accumulated occlusion factor
    mediump float c1 = (A + B + C + D) * 0.07142857142857142857;
    mediump float c2 = (E + F + G + H) * 0.07142857142857142857;
    mediump float c3 = (I + J + K + L) * 0.07142857142857142857;
    mediump float c4 = (M + N) * 0.07142857142857142857;
    mediump float occ = c1 + c2 + c3 + c4;

    // amplify and saturate if necessary
    occ = clamp(occ, 0.0, 1.0);
    FragColor = vec4(occ, 0.0, 0.0, 1.0);
}
