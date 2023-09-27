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
#define MAX_BIN_SEARCH_COUNT 10
#define MAX_RAY_MARCH_COUNT 30
#define STEP 0.05
#define JITT_SCALE 0.01
#define LLIMITER 0.1

uniform sampler2D RT;
uniform sampler2D DepthMap;
uniform sampler2D NormalMap;
uniform sampler2D GlossMap;

uniform mediump mat4 ProjMatrix;
uniform mediump float FarClipDistance;
uniform mediump float NearClipDistance;

mediump vec3 hash(const mediump vec3 a)
{
    mediump vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b. zyx);
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

mediump vec2 BinarySearch(mediump vec3 position, mediump vec3 direction)
{
    for(int i = 0; i < MAX_BIN_SEARCH_COUNT; ++i) {
        mediump vec4 nuv = mul(ProjMatrix, vec4(position, 1.0));
        nuv.xy /= nuv.w;

        mediump float depth = texture2D(DepthMap, nuv.xy).x;
        mediump float delta = position.z - depth;

        direction *= 0.5;

        position += delta > 0.0 ? direction : -direction;
    }

    mediump vec4 nuv = mul(ProjMatrix, vec4(position, 1.0));
    nuv.xy /= nuv.w;

    return nuv.xy;
}

mediump vec2 RayCast(mediump vec3 position, mediump vec3 direction)
{
    direction *= STEP;

    mediump vec4 nuv;

    for (int i = 0; i < MAX_RAY_MARCH_COUNT; ++i) {
        position += direction;

        nuv = mul(ProjMatrix, vec4(position, 1.0));
        nuv.xy /= nuv.w;

        mediump float depth = texture2D(DepthMap, nuv.xy).x;
        mediump float delta = position.z - depth;

        // Is the difference between the starting and sampled depths smaller than the width of the unit cube?
        // We don't want to sample too far from the starting position.
        // We're at least past the point where the ray intersects a surface.
        // Now, determine the values at the precise location of intersection.
        if (FarClipDistance * (direction.z - delta) < 1.2 && delta <= 0.0) {
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

in mediump vec2 vUV0;
in highp vec3 vRay;
void main()
{
    mediump vec2 gloss = texture2D(GlossMap, vUV0).rg;
    mediump float metallic = gloss.r;
    mediump float roughness = gloss.g;
    mediump float clampedDepth = texture2D(DepthMap, vUV0).x;

    vec2 p = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y));
    if (metallic < 0.04 || clampedDepth > 0.5) {
        FragColor = vec4(texture2D(RT, vUV0).rgb, 1.0);
        return;
    }

    mediump vec3 normal = texture2D(NormalMap, vUV0).xyz;
    mediump vec3 viewPos = vRay * texture2D(DepthMap, vUV0).x;
    mediump vec3 jitt = hash(gl_FragCoord.xyz) * roughness * JITT_SCALE;

    // Reflection vector
    mediump vec3 reflected = normalize(reflect(normalize(viewPos), normalize(normal)));
    mediump vec3 position = viewPos;
    mediump vec2 coords = RayCast(position, reflected + jitt);

    mediump float L = length(vRay * texture2D(DepthMap, vUV0).x - viewPos);
    L = clamp(L * LLIMITER, 0.0, 1.0);
    mediump float error = 1.0 - L;
    mediump float fresnel = Fresnel(reflected, normal);
    mediump vec3 color = texture2D(RT, coords.xy).rgb * error * fresnel;

    FragColor = vec4(mix(texture2D(RT, vUV0).rgb, color, metallic), 1.0);
}
