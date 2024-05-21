// created by Andrey Vasiliev

float luminance(const vec3 col) {
    return dot(col, vec3(0.2126, 0.7152, 0.0722));
}

// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
vec3 aces(const vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
vec3 unreal(const highp vec3 x)
{
    return x / (x + 0.155) * 1.019;
}

vec3 tonemap(const highp vec3 x) {
    return unreal(x);
}

/*
 * The input must be in the [0, 1] range.
 */
vec3 Inverse_Tonemap_Unreal(const vec3 x) {
    return (x * -0.155) / (x - 1.019);
}

/**
 * Applies the inverse of the tone mapping operator to the specified HDR or LDR
 * linear RGB color and returns a linear RGB color. The inverse tone mapping operator
 * may be an approximation of the real inverse operation.
 *
 * @public-api
 */
vec3 inverseTonemap(vec3 linear) {
    // Linear input
    linear = clamp(linear, 0.0, 1.0);
    return Inverse_Tonemap_Unreal(pow(linear, vec3(1.0 / 2.2)));
}

/**
 * Applies the inverse of the tone mapping operator to the specified HDR or LDR
 * sRGB (non-linear) color and returns a linear sRGB color. The inverse tone mapping
 * operator may be an approximation of the real inverse operation.
 *
 * @public-api
 */
vec3 inverseTonemapSRGB(vec3 color) {
    // sRGB input
    color = clamp(color, 0.0, 1.0);
    return Inverse_Tonemap_Unreal(color);
}

// Hable 2010, "Filmic Tonemapping Operators"
// Based on Duiker's curve, optimized by Hejl and Burgess-Dawson
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
vec3 filmic(const vec3 x) {
    vec3 c = max(vec3(0.0), x - 0.004);
    return (c * (c * 6.2 + 0.5)) / (c * (c * 6.2 + 1.7) + 0.06);
}
