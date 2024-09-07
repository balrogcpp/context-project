// created by Andrey Vasiliev
// based on https://github.com/OGRECave/ogre/blob/v13.6.4/Samples/Media/DeferredShadingMedia/ssao_ps.glsl

#include "header.glsl"
#include "math.glsl"

uniform sampler2D DepthTex;
uniform sampler2D NormalTex;
uniform mat4 ProjMatrix;
uniform float FarClipDistance;
uniform float NearClipDistance;

const float kernelRadius = 1.0;
const float invKernelSize = 1.0 / kernelRadius;

vec3 hash(const vec3 a)
{
    vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b.zyx);
}

vec3 getScreenSpacePos(const vec2 uv, const vec3 cameraNormal)
{
    float linearDepth = textureLod(DepthTex, uv, 0.0).x;
    return cameraNormal * linearDepth;
}

vec3 GetCameraVec(const vec2 uv)
{
    // Returns the vector from camera to the specified position on the camera plane (uv argument), located one unit away from the camera
    // This vector is not normalized.
    // The nice thing about this setup is that the returned vector from this function can be simply multiplied with the linear depth to get pixel's position relative to camera position.
    // This particular function does not account for camera rotation or position or FOV at all (since we don't need it for AO)
    // TODO: AO is dependent on FOV, this function is not!
    // The outcome of using this simplified function is that the effective AO range is larger when using larger FOV
    // Use something more accurate to get proper FOV-independent world-space range, however you will likely also have to adjust the SSAO constants below
    const float aspect = 1.0;
    return vec3(uv.x * 2.0 - 1.0, -uv.y * 2.0 * aspect + aspect, 1.0);
}

float Falloff(float dist2, float cosh)
{
    return 2.0 * clamp((dist2 - 0.16) / (4.0 - 0.16), 0.0, 1.0);
}

#define MAX_RAND_SAMPLES 14
#define RADIUS 0.21 // 0.105
#define INVSQ3 0.57735026918962576451
#define NUM_BASE_SAMPLES MAX_RAND_SAMPLES

const vec3 RAND_SAMPLES[MAX_RAND_SAMPLES] =
   vec3[](
   vec3(1.0, 0.0, 0.0),
   vec3(-1.0, 0.0, 0.0),
   vec3(0.0, 1.0, 0.0),
   vec3(0.0, -1.0, 0.0),
   vec3(0.0, 0.0, 1.0),
   vec3(0.0, 0.0, -1.0),
   vec3( INVSQ3,  INVSQ3,  INVSQ3),
   vec3(-INVSQ3,  INVSQ3,  INVSQ3),
   vec3( INVSQ3, -INVSQ3,  INVSQ3),
   vec3( INVSQ3,  INVSQ3, -INVSQ3),
   vec3(-INVSQ3, -INVSQ3,  INVSQ3),
   vec3(-INVSQ3,  INVSQ3, -INVSQ3),
   vec3( INVSQ3, -INVSQ3, -INVSQ3),
   vec3(-INVSQ3, -INVSQ3, -INVSQ3)
);

//in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    vec2 uv = 0.5 * gl_FragCoord.xy / vec2(textureSize(DepthTex, 0).xy);
    uv.y = 1.0 - uv.y;

    vec3 normal = textureLod(NormalTex, uv, 0.0).xyz;

   // Depth of the current pixel
   vec3 ray = vRay;

   // random normal lookup from a texture and expand to [-1..1]
   // IN.ray will be distorted slightly due to interpolation
   // it should be normalized here
   float clampedPixelDepth = textureLod(DepthTex, uv, 0.0).x;
   float pixelDepth = clampedPixelDepth * (FarClipDistance - NearClipDistance) + NearClipDistance;
   vec3 viewPos = ray * clampedPixelDepth;
   vec3 randN = normalize(hash(viewPos.xyz));

   // Accumulated occlusion factor
   float occlusion = 0.0;
   for (int i = 0; i < NUM_BASE_SAMPLES; ++i) {
       // Reflected direction to move in for the sphere
       // (based on random samples and a random texture sample)
       // bias the random direction away from the normal
       // this tends to minimize self occlusion
       vec3 randomDir = normalize(reflect(RAND_SAMPLES[i], randN) + normal);

       vec3 oSample = viewPos + randomDir * RADIUS;
       vec4 nuv = mulMat4x4Half3(ProjMatrix, oSample);
       nuv.xy /= nuv.w;

       // Compute occlusion based on the (scaled) Z difference
       float clampedSampleDepth = textureLod(DepthTex, nuv.xy, 0.0).x;
       float sampleDepth = clampedSampleDepth * (FarClipDistance - NearClipDistance) + NearClipDistance;
       float rangeCheck = smoothstep(0.0, 1.0, RADIUS / (pixelDepth - sampleDepth)) * step(oSample.z, clampedSampleDepth);

       // This is a sample occlusion function, you can always play with
       // other ones, like 1.0 / (1.0 + zd * zd) and stuff
       occlusion += clamp(pow(1.0 - rangeCheck, 10.0) + rangeCheck, 0.0, 1.0);
   }

   // normalise
   occlusion /= float(NUM_BASE_SAMPLES);

    FragColor.rgb = vec3(occlusion);
}
