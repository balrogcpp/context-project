// created by Andrey Vasiliev
// SSR based on tutorial by Imanol Fotia
// http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html
// https://gitlab.com/congard/algine/-/blob/v1.6-alpha/src/resources/shaders/ssr/fragment.glsl

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
#include "mosaic.glsl"
#define MAX_BIN_SEARCH_COUNT 10
#define MAX_RAY_MARCH_COUNT 30
#define STEP 0.1
#define JITT_SCALE 0.01
#define LLIMITER 0.1

uniform sampler2D DepthTex;
uniform sampler2D NormalTex;
uniform sampler2D GlossTex;
uniform mediump mat4 ProjMatrix;
uniform mediump float ClipDistance;

mediump vec3 hash(const mediump vec3 a)
{
    mediump vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b. zyx);
}

mediump vec2 BinarySearch(inout mediump vec3 position, mediump vec3 direction)
{
    for(int i = 0; i < MAX_BIN_SEARCH_COUNT; ++i) {
        mediump vec4 nuv = mul(ProjMatrix, vec4(position, 1.0));
        nuv.xy /= nuv.w;

        mediump float depth = texture2D(DepthTex, nuv.xy).x;
        mediump float delta = -position.z / ClipDistance - depth;

        direction *= 0.5;

        position += delta < 0.0 ? direction : -direction;
    }

    mediump vec4 nuv = mul(ProjMatrix, vec4(position, 1.0));
    nuv.xy /= nuv.w;

    return nuv.xy;
}

mediump vec2 RayCast(inout mediump vec3 position, mediump vec3 direction)
{
    direction *= STEP;

    mediump vec4 nuv;
    for (int i = 0; i < MAX_RAY_MARCH_COUNT; ++i) {
        position += direction;

        nuv = mul(ProjMatrix, vec4(position, 1.0));
        nuv.xy /= nuv.w;

        mediump float depth = texture2D(DepthTex, nuv.xy).x;
        mediump float delta = -position.z / ClipDistance - depth;

        // Is the difference between the starting and sampled depths smaller than the width of the unit cube?
        // We don't want to sample too far from the starting position.
        if (delta > 0.0) {
            return BinarySearch(position, direction);
        }
    }

    return nuv.xy;
}

// source: https://www.standardabweichung.de/code/javascript/webgl-glsl-fresnel-schlick-approximation
mediump float Fresnel(const mediump vec3 direction, const mediump vec3 normal)
{
    mediump vec3 halfDirection = normalize(normal + direction);
    mediump float cosine = dot(halfDirection, direction);
    mediump float product = max(cosine, 0.0);
    mediump float factor = 1.0 - pow(product, 5.0);

    return factor;
}

in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    mediump vec2 gloss = texture2D(GlossTex, vUV0).rg;
    mediump float metallic = gloss.r;
    mediump float roughness = gloss.g;
    mediump float clampedDepth = texture2D(DepthTex, vUV0).x;
    mediump vec3 normal = texture2D(NormalTex, vUV0).xyz;

    if (metallic < 0.04 || clampedDepth > 0.5 || clampedDepth < HALF_EPSILON || Null(normal) || ExcludePixel()) {
        return;
    }

    mediump vec3 viewPos = vRay * clampedDepth;
    mediump vec3 jitt = hash(gl_FragCoord.xyz) * roughness * JITT_SCALE;

    // Reflection vector
    mediump vec3 position = viewPos.xyz;
    mediump vec3 reflected = normalize(reflect(normalize(position), normalize(normal)));
    mediump vec2 coords = RayCast(position, reflected + jitt);

    mediump float L = length(viewPos - position);
    L = clamp(L * LLIMITER, 0.0, 1.0);
    mediump float error = 1.0 - L;
    mediump float fresnel = Fresnel(reflected, normal);

    FragColor = vec4(coords, fresnel * error, 1.0);
}
