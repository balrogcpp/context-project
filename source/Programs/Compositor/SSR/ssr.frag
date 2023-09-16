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
#define STEP 0.05
#define MAX_RAY_MARCH_COUNT 30

uniform sampler2D RT;
uniform sampler2D DepthMap;
uniform sampler2D NormalMap;
uniform sampler2D GlossMap;

uniform mediump mat4 ProjMatrix;
uniform mediump mat4 InvViewMatrix;
uniform mediump float FarClipDistance;

mediump vec2 BinarySearch(mediump vec3 position, inout mediump vec3 direction)
{
    mediump vec4 projectedCoord = vec4(-1.0, -1.0, -1.0, -1.0);

    for(int i = 0; i < MAX_BIN_SEARCH_COUNT; ++i) {
        projectedCoord = mul(ProjMatrix, vec4(position, 1.0));
        projectedCoord.xy /= projectedCoord.w;

        mediump float depth = texture2D(DepthMap, projectedCoord.xy).x;
        mediump float delta = position.z - depth;

        direction *= 0.5;

        position += delta > 0.0 ? direction : -direction;
    }

    projectedCoord = mul(ProjMatrix, vec4(position, 1.0));
    projectedCoord.xy /= projectedCoord.w;

    return projectedCoord.xy;
}

mediump vec2 RayCast(mediump vec3 position, inout mediump vec3 direction)
{
    direction *= STEP;

    mediump vec4 projectedCoord = vec4(-1.0, -1.0, -1.0, -1.0);

    for (int i = 0; i < MAX_RAY_MARCH_COUNT; ++i) {
        position += direction;

        projectedCoord = mul(ProjMatrix, vec4(position, 1.0));
        projectedCoord.xy /= projectedCoord.w;

        mediump float depth = texture2D(DepthMap, projectedCoord.xy).x;
        mediump float delta = position.z - depth;

        // if (depth - (position.z - direction.z) < 1.2f)
        // Is the difference between the starting and sampled depths smaller than the width of the unit cube?
        // We don't want to sample too far from the starting position.
        // We're at least past the point where the ray intersects a surface.
        // Now, determine the values at the precise location of intersection.
        if (FarClipDistance * (direction.z - delta) < 1.2 && delta <= 0.0) {
            return BinarySearch(position, direction);
        }
    }

    return projectedCoord.xy;
}

mediump vec3 hash(const mediump vec3 a)
{
    mediump vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b. zyx);
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
    mediump vec2 ssr = texture2D(GlossMap, vUV0).rg;

    if (ssr.r < 0.04) {
        FragColor = vec4(texture2D(RT, vUV0).rgb, 1.0);
        return;
    }

    #define MIN_RAY_STEP 0.2
    #define LLIMITER 0.1
    #define JITT_SCALE 0.01

    mediump vec3 normal = texture2D(NormalMap, vUV0).xyz;
    mediump vec3 viewPos = vRay * texture2D(DepthMap, vUV0).x;
    mediump vec3 worldPos = mul(vec4(viewPos, 1.0), InvViewMatrix).xyz;
    mediump vec3 jitt = hash(worldPos) * ssr.g;

    // Reflection vector
    mediump vec3 reflected = normalize(reflect(normalize(viewPos), normalize(normal))) + jitt * JITT_SCALE;

    // Ray cast
    mediump vec2 coords = RayCast(viewPos, reflected);
    mediump float L = FarClipDistance * abs(texture2D(DepthMap, coords).x - texture2D(DepthMap, vUV0).x);
    L = clamp(L * LLIMITER, 0.0, 1.0);
    mediump float error = 1.0 - L;
    mediump float fresnel = Fresnel(reflected, normal);
    mediump vec3 color = texture2D(RT, coords.xy).rgb * error * fresnel;

    if (coords.x != -1.0) {
        FragColor = vec4(mix(texture2D(RT, vUV0).rgb, color, ssr.r), 1.0);
        return;
    }

    FragColor = vec4(mix(texture2D(RT, vUV0).rgb, vec3(0.0, 1.0, 0.0), ssr.r), 1.0);
}
