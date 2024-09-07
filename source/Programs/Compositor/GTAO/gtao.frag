// created by Andrey Vasiliev
// based on https://github.com/OGRECave/ogre/blob/v13.6.4/Samples/Media/DeferredShadingMedia/ssao_ps.glsl

#include "header.glsl"
#include "math.glsl"

uniform sampler2D DepthTex;
uniform mat4 ProjMatrix;
uniform vec4 ZBufferParams;
uniform float FarClipDistance;
uniform float NearClipDistance;

vec3 hash(const vec3 a)
{
    vec3 b = fract(a * vec3(0.8, 0.8, 0.8));
    b += dot(b, b.yxz + 19.19);
    return fract((b.xxy + b.yxx) * b.zyx);
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
//    return vec3(uv, 1.0);
}

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

vec3 getNormal(const vec2 tc_original)
{
    vec2 tsize = 1.0 / textureSize(DepthTex, 0).xy;

    vec3 P = GetCameraVec(tc_original) * textureLod(DepthTex, tc_original, 0.0).x;
    vec3 Pr = GetCameraVec(tc_original + vec2(tsize.x, 0.0)) * textureLod(DepthTex, tc_original + vec2(tsize.x, 0.0), 0.0).x;
    vec3 Pl = GetCameraVec(tc_original + vec2(-tsize.x, 0.0)) * textureLod(DepthTex, tc_original + vec2(-tsize.x, 0.0), 0.0).x;
    vec3 Pt = GetCameraVec(tc_original + vec2(0.0, tsize.y)) * textureLod(DepthTex, tc_original + vec2(0.0, tsize.y), 0.0).x;
    vec3 Pb = GetCameraVec(tc_original + vec2(0.0, -tsize.y)) * textureLod(DepthTex, tc_original + vec2(0.0, -tsize.y), 0.0).x;
    return normalize(cross(MinDiff(P, Pr, Pl), MinDiff(P, Pt, Pb)) * textureLod(DepthTex, tc_original, 0.0).x);

//    // Depth of the current pixel
//    float dhere = textureLod(DepthTex, tc_original, 0.0).x;
//    // Vector from camera to the current pixel's position
//    vec3 ray = GetCameraVec(tc_original) * dhere;
//
//    const float normalSampleDist = 1.0;
//
//    // Calculate normal from the 4 neighbourhood pixels
//    vec2 uv = tc_original + vec2(tsize.x * normalSampleDist, 0.0);
//    vec3 p1 = ray - GetCameraVec(uv) * textureLod(DepthTex, uv, 0.0).x;
//
//    uv = tc_original + vec2(0.0, tsize.y * normalSampleDist);
//    vec3 p2 = ray - GetCameraVec(uv) * textureLod(DepthTex, uv, 0.0).x;
//
//    uv = tc_original + vec2(-tsize.x * normalSampleDist, 0.0);
//    vec3 p3 = ray - GetCameraVec(uv) * textureLod(DepthTex, uv, 0.0).x;
//
//    uv = tc_original + vec2(0.0, -tsize.y * normalSampleDist);
//    vec3 p4 = ray - GetCameraVec(uv) * textureLod(DepthTex, uv, 0.0).x;
//
//    vec3 normal1 = normalize(cross(p1, p2));
//    vec3 normal2 = normalize(cross(p3, p4));
//
//    return normalize(normal1 + normal2);
}

float Falloff(float dist2, float cosh)
{
    return 2.0 * clamp((dist2 - 0.16) / (4.0 - 0.16), 0.0, 1.0);
}

in highp vec3 vRay;
void main()
{
//    #define MAX_RAND_SAMPLES 14
//    #define RADIUS 0.21 // 0.105
//    #define INVSQ3 0.57735026918962576451
//#ifndef GL_ES
//    #define NUM_BASE_SAMPLES MAX_RAND_SAMPLES
//#else
//    #define NUM_BASE_SAMPLES 6
//#endif
//
//    const vec3 RAND_SAMPLES[MAX_RAND_SAMPLES] =
//        vec3[](
//        vec3(1.0, 0.0, 0.0),
//        vec3(-1.0, 0.0, 0.0),
//        vec3(0.0, 1.0, 0.0),
//        vec3(0.0, -1.0, 0.0),
//        vec3(0.0, 0.0, 1.0),
//        vec3(0.0, 0.0, -1.0),
//        vec3( INVSQ3,  INVSQ3,  INVSQ3),
//        vec3(-INVSQ3,  INVSQ3,  INVSQ3),
//        vec3( INVSQ3, -INVSQ3,  INVSQ3),
//        vec3( INVSQ3,  INVSQ3, -INVSQ3),
//        vec3(-INVSQ3, -INVSQ3,  INVSQ3),
//        vec3(-INVSQ3,  INVSQ3, -INVSQ3),
//        vec3( INVSQ3, -INVSQ3, -INVSQ3),
//        vec3(-INVSQ3, -INVSQ3, -INVSQ3)
//    );
//
//    vec2 size = vec2(textureSize(DepthTex, 0) * 2.0);
//    vec2 uv = gl_FragCoord.xy / size;
//    uv.y = 1.0 - uv.y;
//
//    // random normal lookup from a texture and expand to [-1..1]
//    // IN.ray will be distorted slightly due to interpolation
//    // it should be normalized here
//    float clampedPixelDepth = textureLod(DepthTex, uv, 0.0).x;
//    float pixelDepth = clampedPixelDepth * (FarClipDistance - NearClipDistance) + NearClipDistance;
//    vec3 viewPos = vRay * clampedPixelDepth;
//    vec3 viewPos2 = GetCameraVec(uv) * clampedPixelDepth;
//    vec3 randN = normalize(hash(viewPos.xyz));
//
//    // By computing Z manually, we lose some accuracy under extreme angles
//    // considering this is just for bias, this loss is acceptable
////    vec3 normal = unpack(textureLod(NormalTex, vUV0, 0.0).y);
//    vec3 normal = getNormal(uv);
//
//    // Accumulated occlusion factor
//    float occ = 0.0;
//    for (int i = 0; i < NUM_BASE_SAMPLES; ++i) {
//        // Reflected direction to move in for the sphere
//        // (based on random samples and a random texture sample)
//        // bias the random direction away from the normal
//        // this tends to minimize self occlusion
//        vec3 randomDir = normalize(reflect(RAND_SAMPLES[i], randN) + normal);
//
//        vec3 oSample = viewPos + randomDir * RADIUS;
//        vec4 nuv = mulMat4x4Half3(ProjMatrix, oSample);
//        nuv.xy /= nuv.w;
//
//        // Compute occlusion based on the (scaled) Z difference
//        float clampedSampleDepth = textureLod(DepthTex, nuv.xy, 0.0).x;
//        float sampleDepth = clampedSampleDepth * (FarClipDistance - NearClipDistance) + NearClipDistance;
//        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / (pixelDepth - sampleDepth)) * step(oSample.z, clampedSampleDepth);
//
//        // This is a sample occlusion function, you can always play with
//        // other ones, like 1.0 / (1.0 + zd * zd) and stuff
//        occ += clamp(pow(1.0 - rangeCheck, 10.0) + rangeCheck + 0.6666667 * sqrt(clampedPixelDepth), 0.0, 1.0);
//    }
//
////    // normalise
//    occ /= float(NUM_BASE_SAMPLES);

    // amplify and saturate if necessary
//    FragColor.rgb = vec3(occ, 0.0, 0.0);
}
