// created by Andrey Vasiliev
// SSR based on tutorial by Imanol Fotia
// http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html
// https://gitlab.com/congard/algine/-/blob/v1.6-alpha/src/resources/shaders/ssr/fragment.glsl

#include "header.glsl"
#define MAX_BIN_SEARCH_COUNT 10
#define MAX_RAY_MARCH_COUNT 30
#define STEP 0.05
#define JITT_SCALE 0.01
#define LLIMITER 0.1
#define MIN_RAY_STEP 0.2

uniform sampler2D DepthTex;
uniform sampler2D NormalTex;
uniform sampler2D GlossTex;
uniform sampler2D ColorTex;
uniform mat4 ProjMatrix;
uniform float ClipDistance;

vec2 BinarySearch(vec3 direction, inout vec3 hitCoord)
{
    for(int i = 0; i < MAX_BIN_SEARCH_COUNT; ++i) {
        vec4 nuv = mul(ProjMatrix, vec4(hitCoord, 1.0));
        nuv.xy /= nuv.w;

        float sampleDepth = texture2D(DepthTex, nuv.xy).x * ClipDistance;

        direction *= 0.5;

        hitCoord += (-hitCoord.z - sampleDepth) < 0.0 ? direction : -direction;
    }

    vec4 nuv = mul(ProjMatrix, vec4(hitCoord, 1.0));
    nuv.xy /= nuv.w;

    return nuv.xy;
}

vec2 RayCast(vec3 direction, inout vec3 hitCoord)
{
    direction *= STEP;

    for (int i = 0; i < MAX_RAY_MARCH_COUNT; ++i) {
        hitCoord += direction;

        vec4 nuv  = mul(ProjMatrix, vec4(hitCoord, 1.0));
        nuv.xy /= nuv.w;

        float sampleDepth = texture2D(DepthTex, nuv.xy).x * ClipDistance;
        float dDepth = -hitCoord.z - sampleDepth;

        // Is the difference between the starting and sampled depths smaller than the width of the unit cube?
        // We don't want to sample too far from the starting position.
        if ((direction.z - dDepth) < 1.2 && dDepth >= 0.0) return BinarySearch(hitCoord, direction);
    }

    return vec2(-1.0, -1.0);
}

vec3 hash(const vec3 a)
{
    vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b. zyx);
}

// source: https://www.standardabweichung.de/code/javascript/webgl-glsl-fresnel-schlick-approximation
float Fresnel(const vec3 direction, const vec3 normal)
{
    vec3 halfDirection = normalize(normal + direction);
    float cosine = dot(halfDirection, direction);
    float product = max(cosine, 0.0);
    float factor = 1.0 - pow(product, 5.0);

    return factor;
}

in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    vec2 gloss = texture2D(GlossTex, vUV0).rg;
    float metallic = gloss.r;
    float roughness = gloss.g;
    float clampedPixelDepth = texture2D(DepthTex, vUV0).x;
    vec3 normal = texture2D(NormalTex, vUV0).xyz;

    if (Null(normal) || metallic < HALF_EPSILON) {
        FragColor.rgb = texture2D(ColorTex, vUV0).rgb;
        return;
    }

    vec3 viewPos = vRay * clampedPixelDepth;
    //float jitter = float(int(gl_FragCoord.x + gl_FragCoord.y) & 1) * 0.5;
    vec3 jitter = hash(gl_FragCoord.xyz) * roughness * JITT_SCALE;

    // Reflection vector
    vec3 hitPoint = viewPos;
    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(normal)));
    vec2 hitPixel = RayCast(jitter + reflected * max(-viewPos.z, MIN_RAY_STEP), hitPoint);

    float L = length(viewPos - hitPoint);
    L = clamp(L * LLIMITER, 0.0, 1.0);
    float error = 1.0 - L;

    float fresnel = Fresnel(reflected, normal);

    if (hitPixel != vec2(-1.0, -1.0)) {
        vec3 ssr = texture2D(ColorTex, hitPixel).rgb * error * fresnel;
        FragColor.rgb = ssr;
    } else {
        FragColor.rgb = vec3(0.0, 0.0, 0.0);
    }
}
