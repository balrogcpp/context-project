// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "math.glsl"
precision highp float;


in vec2 vUV0;

uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D ssrValuesMap;
uniform sampler2D colorMap;

uniform mat4 ProjMatrix;
uniform mat4 InvProjMatrix;
uniform mat4 InvViewMatrix;
uniform float FarClipDistance;
uniform float NearClipDistance;


// SSR based on tutorial by Imanol Fotia
// http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html


//----------------------------------------------------------------------------------------------------------------------
vec3 GetPosition(const vec2 uv)
{
    float depth = texture(depthMap, uv).x;
    float z = depth * 2.0 - 1.0;
    z = z * FarClipDistance + NearClipDistance;

    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = InvProjMatrix * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}



//----------------------------------------------------------------------------------------------------------------------
vec2 BinarySearch(vec3 dir, vec3 hitCoord, float dDepth)
{
    float depth = 0.0;
    vec4 projectedCoord = vec4(0.0);

    #define MAX_BIN_SEARCH_COUNT 10

    for (int i = 0; i < MAX_BIN_SEARCH_COUNT; ++i)
    {
        projectedCoord = ProjMatrix * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = GetPosition(projectedCoord.xy).z;

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
vec2 RayCast(vec3 dir, vec3 hitCoord, float dDepth)
{
    dir *= 0.05;

    #define MAX_RAY_MARCH_COUNT 30

    for (int i = 0; i < MAX_RAY_MARCH_COUNT; ++i)
    {
        hitCoord += dir;

        vec4 projectedCoord = ProjMatrix * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        float depth = GetPosition(projectedCoord.xy).z;
        dDepth = hitCoord.z - depth;

        if ((dir.z - dDepth) < 1.2 && dDepth <= 0.0)
        {
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
    float reflectionStrength = texture2D(ssrValuesMap, vUV0).r;

    if (reflectionStrength < F0)
    {
        FragColor.rgb = texture2D(colorMap, vUV0).rgb;
        return;
    }

    vec3 normal = texture2D(normalMap, vUV0).xyz;
    vec3 viewPos = GetPosition(vUV0);

    vec3 worldPos = vec3(vec4(viewPos, 1.0) * InvViewMatrix);
    vec3 jitt = hash(worldPos) * texture2D(ssrValuesMap, vUV0).g;

    // Reflection vector
    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(normal)));

    // Ray cast
    vec3 hitPos = viewPos;
    float dDepth;
    const float minRayStep = 0.2;
    vec2 coords = RayCast(jitt + reflected * max(-viewPos.z, minRayStep), hitPos, dDepth);

    const float llimiter = 0.1;
    float L = length(GetPosition(coords) - viewPos);
    L = clamp(L * llimiter, 0.0, 1.0);
    float error = 1.0 - L;

    float fresnel = Fresnel(reflected, normal);

    vec3 color = texture2D(colorMap, coords.xy).rgb * error * fresnel;

    if (coords.xy != vec2(-1.0))
    {
        FragColor.rgb = mix(texture2D(colorMap, vUV0), vec4(color, 1.0), reflectionStrength).rgb;
        return;
    }

    FragColor.rgb = mix(texture2D(colorMap, vUV0), vec4(vec3(0.0, 0.5, 0.0), 1.0), reflectionStrength).rgb;
}
