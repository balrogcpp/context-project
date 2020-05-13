// Simple PCF
// Number of samples in one dimension (square for total samples)
#define NUM_SHADOW_SAMPLES_1D 2.0
#define SHADOW_FILTER_SCALE 1.0
#define SHADOW_SAMPLES NUM_SHADOW_SAMPLES_1D*NUM_SHADOW_SAMPLES_1D

vec4 offsetSample(vec4 uv, vec2 offset, float invMapSize)
{
    return vec4(uv.xy + offset * invMapSize * uv.w, uv.z, uv.w);
}

float calcDepthShadow(sampler2D shadowMap, vec4 uv, float invShadowMapSize)
{
    // 4-sample PCF
    float shadow = 0.0;
    uv.z = uv.z * 0.5 + 0.5; // convert -1..1 to 0..1
    float compare = uv.z;
    int counter = 0;
    const float radius = 1.0;
    const int iterations = 16;
    const float bias = 0.0;

    //    float shadow_depth = texture2DProj(shadowMap, uv).r - bias;
    //    shadow = (shadow_depth > compare) ? 1.0 : 0.0;

    //    for (float y = -radius; y < radius; y++)
    //    for (float x = -radius; x < radius; x++)
    //    {
    //        vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize;
    //        float depth = texture2D(shadowMap, uv2).r - bias;
    //        if (depth > compare) {
    //            counter++;
    //        }
    //    }
    //    shadow = counter / ( (2.0 * radius) * (2.0 * radius));

    //    for (float y = -radius; y <= radius; y++)
    //    for (float x = -radius; x <= radius; x++)
    //    {
    //        vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize;
    //        float depth = texture2D(shadowMap, uv2).r - bias;
    //        if (depth > compare) {
    //            counter++;
    //        }
    //    }
    //    shadow = counter / ( (2.0 * radius + 1.0 ) * (2.0 * radius + 1.0));

    const vec2 poissonDisk16[16] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.91588581, 0.45771432 ),
    vec2( -0.81544232, -0.87912464 ),
    vec2( -0.38277543, 0.27676845 ),
    vec2( 0.97484398, 0.75648379 ),
    vec2( 0.44323325, -0.97511554 ),
    vec2( 0.53742981, -0.47373420 ),
    vec2( -0.26496911, -0.41893023 ),
    vec2( 0.79197514, 0.19090188 ),
    vec2( -0.24188840, 0.99706507 ),
    vec2( -0.81409955, 0.91437590 ),
    vec2( 0.19984126, 0.78641367 ),
    vec2( 0.14383161, -0.14100790 )
    );

    for (int i = 0; i < iterations; i++)
    {
        vec2 uv2 = uv.xy + poissonDisk16[i] * invShadowMapSize;
        float shadow_depth = texture2D(shadowMap, uv2).r - bias;
        if (shadow_depth > compare) {
            counter++;
        }
    }
    shadow = counter / iterations;

    //      for (float y = -radius; y <= radius; y++)
    //      for (float x = -radius; x <= radius; x++)
    //      for (int i = 0; i < iterations; i++)
    //      {
    //        vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize + poissonDisk16[i] * invShadowMapSize;
    //        float depth = texture2D(shadowMap, uv2).r - bias;
    //        if (depth > compare) {
    //          counter++;
    //        }
    //      }
    //      shadow = counter / ( iterations * (2 * radius + 1 ) * (2 * radius + 1));

//    shadow = clamp(shadow + uShadowColour.r, 0, 1);

    return shadow;
}

float calcDepthShadow_(sampler2D shadowMap, vec4 uv, float invShadowMapSize)
{
    // 4-sample PCF
    float shadow = 0.0;
    float offset = (NUM_SHADOW_SAMPLES_1D/2.0 - 0.5) * SHADOW_FILTER_SCALE;
    uv /= uv.w;
    #ifndef OGRE_REVERSED_Z
    uv.z = uv.z * 0.5 + 0.5;// convert -1..1 to 0..1
    #endif
    for (float y = -offset; y <= offset; y += SHADOW_FILTER_SCALE)
    for (float x = -offset; x <= offset; x += SHADOW_FILTER_SCALE)
    {
        vec2 newUV = uv.xy + vec2(x, y) * invShadowMapSize;
        float depth = texture2D(shadowMap, newUV).x;
        if (depth >= 1.0 || depth >= uv.z)// depth = 1.0 at PSSM end
        shadow += 1.0;
    }
    shadow /= SHADOW_SAMPLES;
    #ifdef OGRE_REVERSED_Z
    shadow = 1.0 - shadow;
    #endif
    return shadow;
}

float calcSimpleShadow(sampler2D shadowMap, vec4 shadowMapPos)
{
    return texture2DProj(shadowMap, shadowMapPos).x;
}
vec4 expand(vec4 v)
{
    return v * 2.0 - 1.0;
}
// From http://substance.io/zauner/porting-vvvv-hlsl-shaders-to-vvvvjs
vec4 lit(float NdotL, float NdotH, float m) {
    float ambient = 1.0;
    float diffuse = max(0.0, NdotL);
    float specular = step(0.0, NdotL) * max(NdotH, 0.0);
    return vec4(ambient, diffuse, specular, 1.0);
}
