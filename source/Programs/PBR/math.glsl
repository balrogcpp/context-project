// created by Andrey Vasiliev

#ifndef MATH_GLSL
#define MATH_GLSL


#ifdef OGRE_GLSL
#include "hlsl2.glsl"
#endif
#ifdef OGRE_HLSL
#include "glsl2.hlsl"
#endif


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
#ifdef OGRE_GLSLES
#define F0 0.089
#else
#define F0 0.045
#endif


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
