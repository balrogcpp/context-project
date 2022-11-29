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

float min3(float a, float b, float c) {
    return min(a, min(b, c));
}

float max3(float a, float b, float c) {
    return max(a, max(b, c));
}

float min3(vec3 a) {
    return min3(a.x, a.y, a.z);
}

float max3(vec3 a) {
    return max3(a.x, a.y, a.z);
}

// https://community.khronos.org/t/saturate/53155
float saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

vec2 saturate(vec2 x) {
    return clamp(x, 0.0, 1.0);
}

vec3 saturate(vec3 x) {
    return clamp(x, 0.0, 1.0);
}

vec4 saturate(vec4 x) {
    return clamp(x, 0.0, 1.0);
}

// https://twitter.com/SebAaltonen/status/878250919879639040
float fsign(float x) {
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

vec2 fsign(vec2 x) {
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

vec3 fsign(vec3 x) {
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

vec4 fsign(vec4 x) {
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
}

#endif // MATH_GLSL
