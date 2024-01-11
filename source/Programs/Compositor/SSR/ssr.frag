// created by Andrey Vasiliev
// SSR based on tutorial by Imanol Fotia
// http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html
// https://gitlab.com/congard/algine/-/blob/v1.6-alpha/src/resources/shaders/ssr/fragment.glsl

#include "header.glsl"
#define MAX_BIN_SEARCH_COUNT 10
#define MAX_RAY_MARCH_COUNT 30
#define STEP 0.1
#define JITT_SCALE 0.01
#define LLIMITER 0.1

uniform sampler2D DepthTex;
uniform sampler2D NormalTex;
uniform sampler2D GlossTex;
uniform mat4 ProjMatrix;
uniform float ClipDistance;

vec3 hash(vec3 a)
{
    vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b. zyx);
}

vec2 BinarySearch(inout vec3 position, vec3 direction)
{
    for(int i = 0; i < MAX_BIN_SEARCH_COUNT; ++i) {
        vec4 nuv = mul(ProjMatrix, vec4(position, 1.0));
        nuv.xy /= nuv.w;
        if (nuv.x < 0.0 || nuv.y < 0.0 || nuv.x > 1.0 || nuv.y > 1.0) return vec2(-1.0, -1.0);

        float depth = texture2D(DepthTex, nuv.xy).x;
        float delta = -position.z / ClipDistance - depth;

        direction *= 0.5;

        position += delta < 0.0 ? direction : -direction;
    }

    vec4 nuv = mul(ProjMatrix, vec4(position, 1.0));
    nuv.xy /= nuv.w;

    return nuv.xy;
}

vec2 RayCast(inout vec3 position, vec3 direction)
{
    direction *= STEP;

    vec4 nuv;
    for (int i = 0; i < MAX_RAY_MARCH_COUNT; ++i) {
        position += direction;

        nuv = mul(ProjMatrix, vec4(position, 1.0));
        nuv.xy /= nuv.w;

        if (nuv.x < 0.0 || nuv.y < 0.0 || nuv.x > 1.0 || nuv.y > 1.0) return vec2(-1.0, -1.0);

        float depth = texture2D(DepthTex, nuv.xy).x;
        float delta = -position.z / ClipDistance - depth;

        // Is the difference between the starting and sampled depths smaller than the width of the unit cube?
        // We don't want to sample too far from the starting position.
        if (delta > 0.0) {
            return BinarySearch(position, direction);
        }
    }

    return nuv.xy;
}

// source: https://www.standardabweichung.de/code/javascript/webgl-glsl-fresnel-schlick-approximation
float Fresnel(vec3 direction, vec3 normal)
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
    float clampedDepth = texture2D(DepthTex, vUV0).x;
    vec3 normal = texture2D(NormalTex, vUV0).xyz;

    if (metallic < 0.04 || clampedDepth > 0.5 || clampedDepth < HALF_EPSILON || Null(normal)) {
        return;
    }

    vec3 viewPos = vRay * clampedDepth;
    vec3 jitt = hash(gl_FragCoord.xyz) * roughness * JITT_SCALE;

    // Reflection vector
    vec3 position = viewPos.xyz;
    vec3 reflected = normalize(reflect(normalize(position), normalize(normal)));
    vec2 uv = RayCast(position, reflected + jitt);

    float L = length(viewPos - position);
    L = clamp(L * LLIMITER, 0.0, 1.0);
    float error = 1.0 - L;
    float fresnel = Fresnel(reflected, normal);

    if (uv.x >= 0.0 && uv.y >= 0.0 && uv.x <= 1.0 && uv.y <= 1.0) {
        FragColor.rgb = vec3(uv, fresnel * error);
    }
}
