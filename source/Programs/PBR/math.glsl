// created by Andrey Vasiliev

#ifndef MATH_GLSL
#define MATH_GLSL


#define HALF_MAX        65504.0 // (2 - 2^-10) * 2^15
#define HALF_MAX_MINUS1 65472.0 // (2 - 2^-9) * 2^15
#define HALF_EPSILON    1.0e-4
#define PI              3.14159265359
#define TWO_PI          6.28318530718
#define FOUR_PI         12.56637061436
#define INV_PI          0.31830988618
#define INV_TWO_PI      0.15915494309
#define INV_FOUR_PI     0.07957747155
#define HALF_PI         1.57079632679
#define INV_HALF_PI     0.636619772367

#define FLT_EPSILON     1.192092896e-07 // Smallest positive number, such that 1.0 + FLT_EPSILON != 1.0
#define FLT_MIN         1.175494351e-38 // Minimum representable positive floating-point number
#define FLT_MAX         3.402823466e+38 // Maximum representable floating-point number

#define M_PI PI
#define F0 0.04


// some hlsl functions as macroses
#define float4 vec4
#define float3 vec3
#define float2 vec2
#define int4 ivec4
#define int3 ivec3
#define int2 ivec2
#define uint4 uvec4
#define uint3 uvec3
#define uint2 uvec2
#define bool4 bvec4
#define bool3 bvec3
#define bool2 bvec2
#define float2x2 mat2x2
#define float2x3 mat3x2
#define float2x4 mat4x2
#define float3x2 mat2x3
#define float3x3 mat3x3
#define float3x4 mat4x3
#define float4x2 mat2x4
#define float4x3 mat3x4
#define float4x4 mat4x4
#define matrix mat4x4
#define static
#define groupshared shared
#define atan2 atan
#define ddx dFdx
#define ddy dFdy
#define ddx_coarse dFdx
#define ddy_coarse dFdy
#define ddx_fine dFdx
#define ddy_fine dFdy
#ifdef GL_ES
#   define mad(a, b, c) ((a) * (b) + (c))
#else
#   define mad fma
#endif
#define lerp mix
#define fmod modf
#define mul(a, b) ((a)*(b))
#define frac fract
#define atan2 atan
#define rsqrt inversesqrt
#define fmod mod
#define lerp mix
#define dst distance
#define countbits bitCount
#define firstbithigh findMSB
#define firstbitlow findLSB
#define reversebits bitfieldReverse
#define tex2D texture2D
#define tex3D texture3D
#define texCube textureCube


float rcp(float x)
{
    return 1.0 / x;
}

vec2 rcp(vec2 x)
{
    return vec2(1.0) / x;
}

vec3 rcp(vec3 x)
{
    return vec3(1.0) / x;
}

vec4 rcp(vec4 x)
{
    return vec4(1.0) / x;
}

// https://community.khronos.org/t/saturate/53155
float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

vec2 saturate(vec2 x)
{
    return clamp(x, 0.0, 1.0);
}

vec3 saturate(vec3 x)
{
    return clamp(x, 0.0, 1.0);
}

vec4 saturate(vec4 x)
{
    return clamp(x, 0.0, 1.0);
}

// extra optimized functions
float min3(float a, float b, float c)
{
    return min(a, min(b, c));
}

float max3(float a, float b, float c)
{
    return max(a, max(b, c));
}

float min3(vec3 a)
{
    return min3(a.x, a.y, a.z);
}

float max3(vec3 a)
{
    return max3(a.x, a.y, a.z);
}

// https://twitter.com/SebAaltonen/status/878250919879639040
float biggerhp(highp float x, highp float y)
{
    return saturate((x - y - HALF_EPSILON) * FLT_MAX);
}

float bigger(mediump float x, mediump float y)
{
    return saturate((x - y - HALF_EPSILON) * HALF_MAX);
}

// https://twitter.com/SebAaltonen/status/878250919879639040
float biggerhp(highp float x)
{
    return saturate((x - FLT_EPSILON) * FLT_MAX);
}

float bigger(mediump float x)
{
    return saturate((x - FLT_EPSILON) * HALF_MAX);
}

float fsinghp(highp float x)
{
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

vec2 fsinghp(highp vec2 x)
{
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

vec3 fsinghp(highp vec3 x)
{
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

vec4 fsinghp(highp vec4 x)
{
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

float fsign(mediump float x)
{
    return saturate(x * HALF_MAX + 0.5) * 2.0 - 1.0;
}

vec2 fsign(mediump vec2 x)
{
    return saturate(x * HALF_MAX + 0.5) * 2.0 - 1.0;
}

vec3 fsign(mediump vec3 x)
{
    return saturate(x * HALF_MAX + 0.5) * 2.0 - 1.0;
}

vec4 fsign(mediump vec4 x)
{
    return saturate(x * HALF_MAX + 0.5) * 2.0 - 1.0;
}

// NaN checker
// /Gic isn't enabled on fxc so we can't rely on isnan() anymore
bool IsNan(float x)
{
    // For some reason the following tests outputs "internal compiler error" randomly on desktop
    // so we'll use a safer but slightly slower version instead :/
    //return (x <= 0.0 || 0.0 <= x) ? false : true;
    return (x < 0.0 || x > 0.0 || x == 0.0) ? false : true;
}

bool AnyIsNan(vec2 x)
{
    return IsNan(x.x) || IsNan(x.y);
}

bool AnyIsNan(vec3 x)
{
    return IsNan(x.x) || IsNan(x.y) || IsNan(x.z);
}

bool AnyIsNan(vec4 x)
{
    return IsNan(x.x) || IsNan(x.y) || IsNan(x.z) || IsNan(x.w);
}

// Clamp HDR value within a safe range
float SafeHDR(float c)
{
    return min(c, HALF_MAX);
}

vec2 SafeHDR(vec2 c)
{
    return min(c, HALF_MAX);
}

vec3 SafeHDR(vec3 c)
{
    return min(c, HALF_MAX);
}

vec4 SafeHDR(vec4 c)
{
    return min(c, HALF_MAX);
}

#endif // MATH_GLSL
