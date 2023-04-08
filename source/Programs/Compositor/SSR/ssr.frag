// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"


in mediump vec2 vUV0;
uniform sampler2D DepthMap;
uniform sampler2D NormalMap;
uniform sampler2D GlossMap;
uniform sampler2D ColorMap;
uniform mediump mat4 ProjMatrix;
uniform mediump mat4 InvProjMatrix;
uniform mediump mat4 InvViewMatrix;


// SSR based on tutorial by Imanol Fotia
// http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html


//----------------------------------------------------------------------------------------------------------------------
vec3 getPositionFromDepth(const vec2 uv, const float depth)
{
    return vec3(InvProjMatrix * vec4(uv, depth, 1.0));
}


//----------------------------------------------------------------------------------------------------------------------
vec3 getPosition(const vec2 uv)
{
    return getPositionFromDepth(uv, texture2D(DepthMap, uv).x);
}


//----------------------------------------------------------------------------------------------------------------------
float getDepth(const vec2 uv)
{
    return getPosition(uv).z;
}


//----------------------------------------------------------------------------------------------------------------------
vec2 BinarySearch(vec3 dir, vec3 hitCoord, float dDepth)
{
    float depth;

    vec4 projectedCoord;

    #define MAX_BIN_SEARCH_COUNT 10

    for(int i = 0; i < MAX_BIN_SEARCH_COUNT; ++i) {
        projectedCoord = ProjMatrix * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = getDepth(projectedCoord.xy);

        dDepth = hitCoord.z - depth;

        dir *= 0.5;

        hitCoord += dDepth > 0.0 ? dir : -dir;
    }

    projectedCoord = ProjMatrix * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

    return vec2(projectedCoord.xy);
}


//----------------------------------------------------------------------------------------------------------------------
vec2 RayMarch(vec3 dir, vec3 hitCoord, float dDepth)
{
    #define STEP 0.05

    dir *= STEP;

    #define MAX_RAY_MARCH_COUNT 30

    for (int i = 0; i < MAX_RAY_MARCH_COUNT; ++i) {
        hitCoord += dir;

        vec4 projectedCoord = ProjMatrix * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        float depth = getDepth(projectedCoord.xy);
        dDepth = hitCoord.z - depth;

        if ((dir.z - dDepth) < 1.2 && dDepth <= 0.0) {
            return BinarySearch(dir, hitCoord, dDepth);
        }
    }

    return vec2(-1.0);
}


//----------------------------------------------------------------------------------------------------------------------
vec3 hash(vec3 a)
{
    a = fract(a * vec3(0.8));
    a += dot(a, a.yxz + 19.19);
    return fract((a.xxy + a.yxx) * a. zyx);
}


// source: https://www.standardabweichung.de/code/javascript/webgl-glsl-fresnel-schlick-approximation
//----------------------------------------------------------------------------------------------------------------------
float Fresnel(const vec3 direction, const vec3 normal)
{
    vec3 halfDirection = normalize(normal + direction);

    float cosine = dot(halfDirection, direction);
    float product = max(cosine, 0.0);
    float factor = 1.0 - pow(product, 5.0);

    return factor;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    vec2 ssr = texture2D(GlossMap, vUV0).rg;
    float reflectionStrength = ssr.r;

    if (reflectionStrength < 0.04) {
        FragColor.rgb = texture2D(ColorMap, vUV0).rgb;
        return;
    }

    vec3 normal = texture2D(NormalMap, vUV0).xyz;
    vec3 viewPos = getPosition(vUV0);

    vec3 worldPos = vec3(InvViewMatrix * vec4(viewPos, 1.0));
    vec3 jitt = hash(worldPos) * ssr.g;

    // Reflection vector
    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(normal)));

    // Ray cast
    #define MIN_RAY_STEP 0.2
    #define LLIMITER 0.1

    vec3 hitPos = viewPos;
    float dDepth;

    vec2 coords = RayMarch(jitt + reflected, hitPos, dDepth);

    float L = length(getPosition(coords) - viewPos);
    L = clamp(L * LLIMITER, 0.0, 1.0);
    float error = 1.0 - L;

    float fresnel = Fresnel(reflected, normal);

    vec3 color = texture2D(ColorMap, coords.xy).rgb * error * fresnel;

    if (coords.xy != vec2(-1.0))
        FragColor.rgb = mix(texture2D(ColorMap, vUV0), vec4(color, 1.0), reflectionStrength).rgb;
    else
        FragColor.rgb = mix(texture2D(ColorMap, vUV0), vec4(vec3(0.0, 0.5, 0.0), 1.0), reflectionStrength).rgb;
}
