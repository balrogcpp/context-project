// created by Andrey Vasiliev

#ifndef TONEMAP_GLSL
#define TONEMAP_GLSL

float luminance(const vec3 linear)
{
    return dot(linear, vec3(0.2126, 0.7152, 0.0722));
}

// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
vec3 aces(const vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

vec3 uncharted2Tonemap(const vec3 x)
{
    const float A = 0.22;
    const float B = 0.3;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.01;
    const float F = 0.30;
    const float W = 11.2;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 uncharted2(const vec3 color)
{
    const float W = 11.2;
    const float exposureBias = 1.0;

    vec3 curr = uncharted2Tonemap(exposureBias * color);
    return curr / uncharted2Tonemap(vec3(W, W, W));
}

// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
highp vec3 unreal(const highp vec3 x)
{
    return x / (x + 0.155) * 1.019;
}

// Hable 2010, "Filmic Tonemapping Operators"
// Based on Duiker's curve, optimized by Hejl and Burgess-Dawson
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
vec3 filmic(const vec3 x)
{
    vec3 c = max(vec3(0.0), x - 0.004);
    return (c * (c * 6.2 + 0.5)) / (c * (c * 6.2 + 1.7) + 0.06);
}

#endif // TONEMAP_GLSL
