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
#ifdef GL_ES
#define F0 0.089
#else
#define F0 0.045
#endif

#define f3_f(c) (dot(round((c) * 255.0), vec3(65536.0, 256.0, 1.0)))
#define f_f3(f) (frac((f) / vec3(16777216.0, 65536.0, 256.0)))

vec2 oct_encode(const vec3 n)
{
    float p = sqrt(n.z * 8.0 + 8.0);
    return vec2(n.xy / p + 0.5);
}

vec3 oct_decode(const vec2 enc)
{
    vec2 fenc = enc * 4.0 - 2.0;
    float f = dot(fenc, fenc);
    float g = sqrt(1.0 - f * 0.25);
    vec3 n;
    n.xy = fenc * g;
    n.z = 1.0 - f * 0.5;
    return n;
}

// https://twitter.com/SebAaltonen/status/878250919879639040
float fstep(float x, float y)
{
    return saturate(((x - y) - HALF_EPSILON) * HALF_MAX);
}

float fsign(float x)
{
    return saturate(x * HALF_MAX + 0.5) * 2.0 - 1.0;
}

float max3(const vec3 a)
{
    return max(max(a.x, a.y), a.z);
}

bool Any(const vec2 x)
{
    return x.x > 0.0 || x.y > 0.0;
}

bool Any(const vec3 x)
{
    return x.x > 0.0 || x.y > 0.0 || x.z > 0.0;
}

bool Any(const vec4 x)
{
    return x.x > 0.0 || x.y > 0.0 || x.z > 0.0 || x.w > 0.0;
}

bool Null(const vec2 x)
{
    return x.x == 0.0 && x.y == 0.0;
}

bool Null(const vec3 x)
{
    return x.x == 0.0 && x.y == 0.0 && x.z == 0.0;
}

bool Null(const vec4 x)
{
    return x.x == 0.0 && x.y == 0.0 && x.z == 0.0 && x.w == 0.0;
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

bool IsNan(const vec2 x)
{
    return IsNan(x.x) || IsNan(x.y);
}

bool IsNan(const vec3 x)
{
    return IsNan(x.x) || IsNan(x.y) || IsNan(x.z);
}

bool IsNan(const vec4 x)
{
    return IsNan(x.x) || IsNan(x.y) || IsNan(x.z) || IsNan(x.w);
}

// Clamp HDR value within a safe range
float SafeHDR(float c)
{
    return min(c, HALF_MAX);
}

vec2 SafeHDR(const vec2 c)
{
    return min(c, HALF_MAX);
}

vec3 SafeHDR(const vec3 c)
{
    return min(c, HALF_MAX);
}

vec4 SafeHDR(const vec4 c)
{
    return min(c, HALF_MAX);
}

#endif // MATH_GLSL
