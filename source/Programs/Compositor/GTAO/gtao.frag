// created by Andrey Vasiliev
// based on https://github.com/OGRECave/ogre/blob/v13.6.4/Samples/Media/DeferredShadingMedia/ssao_ps.glsl

#include "header.glsl"
#include "math.glsl"

uniform sampler2D DepthTex;
//uniform sampler2D NormalTex;
uniform mat4 ProjMatrix;
uniform float FarClipDistance;
uniform float NearClipDistance;
uniform vec4 ZBufferParams;


// Used to get vector from camera to pixel
//const float aspect 1.0;
// These are offsets that change every frame, results are accumulated using temporal filtering in a separate shader
const float aspect = 1.0;
const float angleOffset = 1.0;
const float spacialOffset = 1.0;

#define PI 3.1415926535897932384626433832795
#define PI_HALF 1.5707963267948966192313216916398

float Linear01Depth(const highp float z)
{
    return 1.0 / (z * ZBufferParams.x + ZBufferParams.y);
}

// [Eberly2014] GPGPU Programming for Games and Science
float GTAOFastAcos(float x)
{
    float res = -0.156583 * abs(x) + PI_HALF;
    res *= sqrt(1.0 - abs(x));
    return x >= 0.0 ? res : PI - res;
}

float IntegrateArc(float h1, float h2, float n)
{
    float cosN = cos(n);
    float sinN = sin(n);
    return 0.25 * (-cos(2.0 * h1 - n) + cosN + 2.0 * h1 * sinN - cos(2.0 * h2 - n) + cosN + 2.0 * h2 * sinN);
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
    return vec3(uv.x * 2.0 - 1.0, -uv.y * 2.0 * aspect + aspect, 1.0);
}

#define SSAO_LIMIT 10
#define SSAO_SAMPLES 4
#define SSAO_RADIUS 2.5
#define SSAO_FALLOFF 1.5
#define SSAO_THICKNESSMIX 0.2
#define SSAO_MAX_STRIDE 3

void SliceSample(const vec2 tc_base, const vec2 aoDir, int i, float targetMip, const vec3 ray, const vec3 v, inout float closest)
{
    vec2 uv = tc_base + aoDir * float(i);
//    float depth = textureLod(DepthTex, uv, targetMip).x;
    float depth = Linear01Depth(textureLod(DepthTex, uv, 0.0).x);
    // Vector from current pixel to current slice sample
    vec3 p = GetCameraVec(uv) * depth - ray;
    // Cosine of the horizon angle of the current sample
    float current = dot(v, normalize(p));
    // Linear falloff for samples that are too far away from current pixel
    float falloff = clamp((SSAO_RADIUS - length(p)) / SSAO_FALLOFF, 0.0, 1.0);
    if(current > closest)
    closest = mix(closest, current, falloff);
    // Helps avoid overdarkening from thin objects
    closest = mix(closest, current, SSAO_THICKNESSMIX * falloff);
}

// https://github.com/nvpro-samples/gl_ssao/blob/f6b010dc7a05346518cd13d3368d8d830a382ed9/hbao.frag.glsl

//vec3 UVToView(vec2 uv, float eye_z)
//{
//    return vec3((uv * control.projInfo.xy + control.projInfo.zw) * (control.projOrtho != 0 ? 1. : eye_z), eye_z);
//}

//vec3 FetchViewPos(vec2 UV)
//{
//    float ViewDepth = textureLod(texLinearDepth,UV,0).x;
//    return UVToView(UV, ViewDepth);
//}

vec3 MinDiff(const vec3 P, const vec3 Pr, const vec3 Pl)
{
    vec3 V1 = Pr - P;
    vec3 V2 = P - Pl;
    return (dot(V1,V1) < dot(V2,V2)) ? V1 : V2;
}

vec3 getNormal(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 P = GetCameraVec(uv) * Linear01Depth(textureLod(tex, uv, 0.0).x);
    vec3 Pr = GetCameraVec(uv + vec2(tsize.x, 0.0)) * Linear01Depth(textureLod(tex, uv + tsize * vec2(1.0, 0.0), 0.0).x);
    vec3 Pl = GetCameraVec(uv + vec2(-tsize.x, 0.0)) * Linear01Depth(textureLod(tex, uv + tsize * vec2(-1.0, 0.0), 0.0).x);
    vec3 Pt = GetCameraVec(uv + vec2(0.0, tsize.y)) * Linear01Depth(textureLod(tex, uv + tsize * vec2(0.0, 1.0), 0.0).x);
    vec3 Pb = GetCameraVec(uv + vec2(0.0, -tsize.y)) * Linear01Depth(textureLod(tex, uv + tsize * vec2(0.0, -1.0), 0.0).x);

    return normalize(cross(MinDiff(P, Pr, Pl), MinDiff(P, Pt, Pb)));
}

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

float Falloff(float dist2, float cosh)
{
    return 2.0 * clamp((dist2 - 0.16) / (4.0 - 0.16), 0.0, 1.0);
}
/*
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
*/
//in highp vec3 vRay;
void main()
{
    vec2 viewsizediv = 1.0 / vec2(textureSize(DepthTex, 0));
    vec2 uv = 2.0 * gl_FragCoord.xy * viewsizediv;

    vec3 normal = getNormal(DepthTex, uv, viewsizediv);
    normal.z = -normal.z;

    // Depth of the current pixel
    float dhere = textureLod(DepthTex, uv, 0.0).x;
    // Vector from camera to the current pixel's position
    vec3 ray = GetCameraVec(uv) * dhere;

/*
    // Depth of the current pixel
    vec3 ray = vRay;

    // random normal lookup from a texture and expand to [-1..1]
    // IN.ray will be distorted slightly due to interpolation
    // it should be normalized here
    float clampedPixelDepth = textureLod(DepthTex, uv, 0.0).x;
    float pixelDepth = clampedPixelDepth * (FarClipDistance - NearClipDistance) + NearClipDistance;
    vec3 viewPos = ray * clampedPixelDepth;
    vec3 randN = normalize(hash(viewPos.xyz));

    // Accumulated visibility factor
    float visibility = 0.0;
    for (int i = 0; i < NUM_BASE_SAMPLES; ++i) {
        // Reflected direction to move in for the sphere
        // (based on random samples and a random texture sample)
        // bias the random direction away from the normal
        // this tends to minimize self visibility
        vec3 randomDir = normalize(reflect(RAND_SAMPLES[i], randN) + normal);

        vec3 oSample = viewPos + randomDir * RADIUS;
        vec4 nuv = mulMat4x4Half3(ProjMatrix, oSample);
        nuv.xy /= nuv.w;

        // Compute visibility based on the (scaled) Z difference
        float clampedSampleDepth = textureLod(DepthTex, nuv.xy, 0.0).x;
        float sampleDepth = clampedSampleDepth * (FarClipDistance - NearClipDistance) + NearClipDistance;
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / (pixelDepth - sampleDepth)) * step(oSample.z, clampedSampleDepth);

        // This is a sample visibility function, you can always play with
        // other ones, like 1.0 / (1.0 + zd * zd) and stuff
        visibility += clamp(pow(1.0 - rangeCheck, 10.0) + rangeCheck, 0.0, 1.0);
    }

    // normalise
    visibility /= float(NUM_BASE_SAMPLES);
*/

    // Calculate the distance between samples (direction vector scale) so that the world space AO radius remains constant but also clamp to avoid cache trashing
    // viewsizediv = vec2(1.0 / sreenWidth, 1.0 / screenHeight)
    float stride = min((1.0 / length(ray)) * float(SSAO_LIMIT), float(SSAO_MAX_STRIDE));
    vec2 dirMult = viewsizediv.xy * stride;
    // Get the view vector (normalized vector from pixel to camera)
    vec3 v = normalize(-ray);

    // Calculate slice direction from pixel's position
    float dirAngle = (PI / 16.0) * (float((int(gl_FragCoord.x) + int(gl_FragCoord.y) & 3) << 2) + float(int(gl_FragCoord.x) & 3)) + angleOffset;
    vec2 aoDir = dirMult * vec2(sin(dirAngle), cos(dirAngle));

    // Project world space normal to the slice plane
    vec3 toDir = GetCameraVec(uv + aoDir);
    vec3 planeNormal = normalize(cross(v, -toDir));
    vec3 projectedNormal = normal - planeNormal * dot(normal, planeNormal);

    // Calculate angle n between view vector and projected normal vector
    vec3 projectedDir = normalize(normalize(toDir) + v);
    float n = GTAOFastAcos(dot(-projectedDir, normalize(projectedNormal))) - PI_HALF;

    // Init variables
    float c1 = -1.0;
    float c2 = -1.0;

    vec2 tc_base = uv + aoDir * (0.25 * float((int(gl_FragCoord.y) - int(gl_FragCoord.x)) & 3) - 0.375 + spacialOffset);

    const float minMip = 0.0;
    const float maxMip = 3.0;
    const float mipScale = 1.0 / 12.0;

//    float targetMip = floor(clamp(pow(stride, 1.3) * mipScale, minMip, maxMip));
    float targetMip = 0.0;

    // Find horizons of the slice
    for(int i = -1; i >= -SSAO_SAMPLES; i--)
    {
        SliceSample(tc_base, aoDir, i, targetMip, ray, v, c1);
    }
    for(int i = 1; i <= SSAO_SAMPLES; i++)
    {
        SliceSample(tc_base, aoDir, i, targetMip, ray, v, c2);
    }

    // Finalize
    float h1a = -GTAOFastAcos(c1);
    float h2a = GTAOFastAcos(c2);

    // Clamp horizons to the normal hemisphere
    float h1 = n + max(h1a - n, -PI_HALF);
    float h2 = n + min(h2a - n, PI_HALF);

    float visibility = mix(1.0, IntegrateArc(h1, h2, n), length(projectedNormal));
    FragColor.rgb = vec3(normal);
}
