// created by Andrey Vasiliev

#ifndef MATH_GLSL
#define MATH_GLSL

#define HALF_MAX        65504.0 // (2 - 2^-10) * 2^15
#define HALF_MAX_MINUS1 65472.0 // (2 - 2^-9) * 2^15
#define EPSILON         1.0e-4
#define PI              3.14159265359
#define TWO_PI          6.28318530718
#define FOUR_PI         12.56637061436
#define INV_PI          0.31830988618
#define INV_TWO_PI      0.15915494309
#define INV_FOUR_PI     0.07957747155
#define HALF_PI         1.57079632679
#define INV_HALF_PI     0.636619772367

#define M_PI PI
#define F0 0.04

#define FLT_EPSILON     1.192092896e-07 // Smallest positive number, such that 1.0 + FLT_EPSILON != 1.0
#define FLT_MIN         1.175494351e-38 // Minimum representable positive floating-point number
#define FLT_MAX         3.402823466e+38 // Maximum representable floating-point number

float rcp(float value)
{
    return 1.0 / value;
}

#ifdef GL_ES
#define mad(a, b, c) (a * b + c)
#endif

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

// https://twitter.com/SebAaltonen/status/878250919879639040
float fsign(float x)
{
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

vec2 fsign(vec2 x)
{
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

vec3 fsign(vec3 x)
{
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

vec4 fsign(vec4 x)
{
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

// Using pow often result to a warning like this
// "pow(f, e) will not work for negative f, use abs(f) or conditionally handle negative values if you expect them"
// PositivePow remove this warning when you know the value is positive and avoid inf/NAN.
float PositivePow(float base, float power)
{
    return pow(max(abs(base), float(FLT_EPSILON)), power);
}

vec2 PositivePow(vec2 base, vec2 power)
{
    return pow(max(abs(base), vec2(FLT_EPSILON, FLT_EPSILON)), power);
}

vec3 PositivePow(vec3 base, vec3 power)
{
    return pow(max(abs(base), vec3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON)), power);
}

vec4 PositivePow(vec4 base, vec4 power)
{
    return pow(max(abs(base), vec4(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON)), power);
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

// Interleaved gradient function from Jimenez 2014
// http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
float GradientNoise(vec2 uv, vec2 params)
{
    uv = floor(uv * params.xy);
    float f = dot(vec2(0.06711056, 0.00583715), uv);
    return fract(52.9829189 * fract(f));
}

#endif // MATH_GLSL
