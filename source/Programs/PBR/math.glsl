// created by Andrey Vasiliev

#define HALF_MAX        65504.0 // (2 - 2^-10) * 2^15
#define HALF_EPSILON    1.0e-4
#define PI              3.14159265359
// Clamp HDR value within a safe range
#define SafeHDR(x) min(x, 65504.0)

//float sq(float x) {
//    return x * x;
//}

// https://twitter.com/SebAaltonen/status/878250919879639040
float fstep(float x, float y) {
    return saturate(((y - x) - HALF_EPSILON) * HALF_MAX);
}

float fsign(float x) {
    return saturate(x * HALF_MAX + 0.5) * 2.0 - 1.0;
}

// https://github.com/google/filament/blob/1c693e24cf0c101ab3e21b137e95874117ce6b91/shaders/src/common_math.glsl#L118

//------------------------------------------------------------------------------
// Matrix and quaternion operations
//------------------------------------------------------------------------------

/**
 * Multiplies the specified 3-component vector by the 4x4 matrix (m * v) in
 * high precision.
 *
 * @public-api
 */
highp vec4 mulMat4x4Float3(const highp mat4 m, const highp vec3 v) {
    return v.x * m[0] + (v.y * m[1] + (v.z * m[2] + m[3]));
}
vec4 mulMat4x4Half3(const mat4 m, const vec3 v) {
    return v.x * m[0] + (v.y * m[1] + (v.z * m[2] + m[3]));
}

/**
 * Multiplies the specified 3-component vector by the 3x3 matrix (m * v) in
 * high precision.
 *
 * @public-api
 */
highp vec3 mulMat3x3Float3(const highp mat4 m, const highp vec3 v) {
    return v.x * m[0].xyz + (v.y * m[1].xyz + (v.z * m[2].xyz));
}
vec3 mulMat3x3Half3(const mat4 m, const vec3 v) {
    return v.x * m[0].xyz + (v.y * m[1].xyz + (v.z * m[2].xyz));
}
