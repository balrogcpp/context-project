// created by Andrey Vasiliev

#ifndef TONEMAP_GLSL
#define TONEMAP_GLSL

// https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color
// https://en.wikipedia.org/wiki/Relative_luminance
highp float luminance(const highp vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
highp vec3 aces(const highp vec3 x)
{
    const highp float a = 2.51;
    const highp float b = 0.03;
    const highp float c = 2.43;
    const highp float d = 0.59;
    const highp float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

highp vec3 uncharted2Tonemap(const highp vec3 x)
{
    const highp float A = 0.22;
    const highp float B = 0.3;
    const highp float C = 0.10;
    const highp float D = 0.20;
    const highp float E = 0.01;
    const highp float F = 0.30;
    const highp float W = 11.2;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

highp vec3 uncharted2(const highp vec3 color)
{
    const highp float W = 11.2;
    const highp float exposureBias = 1.0;

    highp vec3 curr = uncharted2Tonemap(exposureBias * color);
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
highp vec3 filmic(const highp vec3 x)
{
    highp vec3 c = max(vec3(0.0), x - 0.004);
    return (c * (c * 6.2 + 0.5)) / (c * (c * 6.2 + 1.7) + 0.06);
}

#endif // TONEMAP_GLSL
