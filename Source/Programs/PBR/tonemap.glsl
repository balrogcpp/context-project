// created by Andrey Vasiliev

#define TONEMAP_UNREAL 1
#define TONEMAP_FILMIC 2
#ifdef GL_ES
    #define USE_TONEMAP TONEMAP_UNREAL
#else
    #define USE_TONEMAP TONEMAP_FILMIC
#endif

vec3 Tonemap_Unreal(const vec3 x) {
    // Unreal, Documentation: "Color Grading"
    // Adapted to be close to Tonemap_ACES, with similar range
    // Gamma 2.2 correction is baked in, don't use with sRGB conversion!
    return x / (x + 0.155) * 1.019;
}

/*
 * The input must be in the [0, 1] range.
 */
vec3 Inverse_Tonemap_Unreal(const vec3 x) {
    return (x * -0.155) / (x - 1.019);
}

vec3 Tonemap_Filmic(const vec3 x) {
    // Hable 2010, "Filmic Tonemapping Operators"
    // Based on Duiker's curve, optimized by Hejl and Burgess-Dawson
    // Gamma 2.2 correction is baked in, don't use with sRGB conversion!
    vec3 c = max(vec3(0.0, 0.0, 0.0), x - 0.004);
    return (c * (c * 6.2 + 0.5)) / (c * (c * 6.2 + 1.7) + 0.06);
}

/*
 * The input must be in the [0, 1] range.
 */
vec3 Inverse_Tonemap_Filmic(const vec3 x) {
    return (0.03 - 0.59 * x - sqrt(0.0009 + 1.3702 * x - 1.0127 * x * x)) / (-5.02 + 4.86 * x);
}

vec3 tonemap(const vec3 x) {
#if USE_TONEMAP == TONEMAP_FILMIC
    return Tonemap_Filmic(x);
#elif USE_TONEMAP == TONEMAP_UNREAL
    return Tonemap_Unreal(x);
#endif
}

/**
 * Applies the inverse of the tone mapping operator to the specified HDR or LDR
 * sRGB (non-linear) color and returns a linear sRGB color. The inverse tone mapping
 * operator may be an approximation of the real inverse operation.
 *
 * @public-api
 */
vec3 inverseTonemapSRGB(const vec3 color) {
    // sRGB input
#if USE_TONEMAP == TONEMAP_FILMIC
    return Inverse_Tonemap_Filmic(pow(clamp(color, 0.0, 1.0), vec3(2.2, 2.2, 2.2)));
#elif USE_TONEMAP == TONEMAP_UNREAL
    return Inverse_Tonemap_Unreal(clamp(color, 0.0, 1.0));
#endif
}

/**
 * Applies the inverse of the tone mapping operator to the specified HDR or LDR
 * linear RGB color and returns a linear RGB color. The inverse tone mapping operator
 * may be an approximation of the real inverse operation.
 *
 * @public-api
 */
vec3 inverseTonemap(const vec3 linear) {
    // Linear input
#if USE_TONEMAP == TONEMAP_FILMIC
    return Inverse_Tonemap_Filmic(clamp(linear, 0.0, 1.0));
#elif USE_TONEMAP == TONEMAP_UNREAL
    return Inverse_Tonemap_Unreal(pow(clamp(linear, 0.0, 1.0), vec3(1.0/2.2, 1.0/2.2, 1.0/2.2)));
#endif
}

vec3 Tonemap_ACES(const vec3 x) {
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}
