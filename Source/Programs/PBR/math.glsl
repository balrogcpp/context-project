// created by Andrey Vasiliev
//? #version 400

#define HALF_MAX        65504.0 // (2 - 2^-10) * 2^15
#define HALF_EPSILON    1.0e-4
#define PI              3.14159265359

// Clamp HDR value within a safe range
#define SafeHDR(x) min(x, 65504.0)
#define saturate(x) clamp(x, 0.0, 1.0)

// https://twitter.com/SebAaltonen/status/878250919879639040
float fsign(const float x) {
    return clamp(x * HALF_MAX + 0.5, 0.0, 1.0) * 2.0 - 1.0;
}

float fstep(const float x, const float y) {
    return clamp(((y - x) - HALF_EPSILON) * HALF_MAX, 0.0, 1.0);
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
//mediump vec4 mulMat4x4Half3(const mediump mat4 m, const mediump vec3 v) {
//    return v.x * m[0] + (v.y * m[1] + (v.z * m[2] + m[3]));
//}

/**
 * Multiplies the specified 3-component vector by the 3x3 matrix (m * v) in
 * high precision.
 *
 * @public-api
 */
highp vec3 mulMat3x3Float3(const highp mat4 m, const highp vec3 v) {
    return v.x * m[0].xyz + (v.y * m[1].xyz + (v.z * m[2].xyz));
}
//mediump vec3 mulMat3x3Half3(const mediump mat4 m, const mediump vec3 v) {
//    return v.x * m[0].xyz + (v.y * m[1].xyz + (v.z * m[2].xyz));
//}

/**
 * Extracts the normal vector of the tangent frame encoded in the specified quaternion.
 */
void toTangentFrame(const highp vec4 q, out highp vec3 n) {
    n = vec3( 0.0,  0.0,  1.0) +
    vec3( 2.0, -2.0, -2.0) * q.x * q.zwx +
    vec3( 2.0,  2.0, -2.0) * q.y * q.wzy;
}

/**
 * Extracts the normal and tangent vectors of the tangent frame encoded in the
 * specified quaternion.
 */
void toTangentFrame(const highp vec4 q, out highp vec3 n, out highp vec3 t) {
    toTangentFrame(q, n);
    t = vec3( 1.0,  0.0,  0.0) +
    vec3(-2.0,  2.0, -2.0) * q.y * q.yxw +
    vec3(-2.0,  2.0,  2.0) * q.z * q.zwx;
}
