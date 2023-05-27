// created by Andrey Vasiliev

#ifndef TONEMAP_GLSL
#define TONEMAP_GLSL

// https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color
// https://en.wikipedia.org/wiki/Relative_luminance
highp float luminance(const highp vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

// https://www.w3.org/TR/AERT/#color-contrast
highp float luminance2(const highp vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

// https://alienryderflex.com/hsp.html
highp float luminance3(const highp vec3 color)
{
    return sqrt(dot(color * color, vec3(0.2126, 0.7152, 0.0722)));
}

highp float luminance4(const highp vec3 color)
{
    return sqrt(dot(color * color, vec3(0.2126, 0.7152, 0.0722)));
}

highp vec3 expose(const highp vec3 color, const highp float exposure)
{
    return vec3(1.0, 1.0, 1.0) - exp(-color * exposure);
}

highp float expose(const highp float color, const highp float exposure)
{
    return 1.0 - exp(-color * exposure);
}

highp vec3 expose2(const highp vec3 color, const highp vec3 exposure)
{
    return exposure.x / exp(clamp(color, exposure.y, exposure.z));
}

highp float expose2(const highp float color, const highp vec3 exposure)
{
    return exposure.x / exp(clamp(color, exposure.y, exposure.z));
}

highp vec3 tonemap(const highp vec3 inColour, const highp float lum)
{
    const highp float middle_grey = 0.72;
    const highp float fudge = 0.001;
    const highp float L_white = 1.5;

    // From Reinhard et al
    // "Photographic Tone Reproduction for Digital Images"

    // Initial luminence scaling (equation 2)
    highp vec3 color = inColour * (middle_grey / (fudge + lum));

    // Control white out (equation 4 nom)
    color *= (1.0 + color / L_white);

    // Final mapping (equation 4 denom)
    color /= (1.0 + color);

    return color;
}

// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
highp vec3 aces(const highp vec3 x) {
    const highp float a = 2.51;
    const highp float b = 0.03;
    const highp float c = 2.43;
    const highp float d = 0.59;
    const highp float e = 0.14;

    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

highp float aces(const highp float x) {
    const highp float a = 2.51;
    const highp float b = 0.03;
    const highp float c = 2.43;
    const highp float d = 0.59;
    const highp float e = 0.14;

    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// Filmic Tonemapping Operators http://filmicworlds.com/blog/filmic-tonemapping-operators/
highp vec3 filmic(const highp vec3 x) {
    highp vec3 X = max(vec3(0.0, 0.0, 0.0), x - 0.004);
    highp vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
    return result;
}

highp float filmic(const highp float x) {
    highp float X = max(0.0, x - 0.004);
    highp float result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
    return result;
}

// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"
highp vec3 lottes(const highp vec3 x) {
    const highp vec3 a = vec3(1.6, 1.6, 1.6);
    const highp vec3 d = vec3(0.977, 0.977, 0.977);
    const highp vec3 hdrMax = vec3(8.0, 8.0, 8.0);
    const highp vec3 midIn = vec3(0.18, 0.18, 0.18);
    const highp vec3 midOut = vec3(0.267, 0.267, 0.267);

    const highp vec3 b =
        (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
    const highp vec3 c =
        (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    return pow(x, a) / (pow(x, a * d) * b + c);
}

highp float lottes(const highp float x) {
    const highp float a = 1.6;
    const highp float d = 0.977;
    const highp float hdrMax = 8.0;
    const highp float midIn = 0.18;
    const highp float midOut = 0.267;

    const highp float b =
        (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
    const highp float c =
        (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    return pow(x, a) / (pow(x, a * d) * b + c);
}

highp vec3 reinhard(const highp vec3 x) {
    return x / (1.0 + x);
}

highp float reinhard(const highp float x) {
    return x / (1.0 + x);
}

highp vec3 reinhard2(const highp vec3 x) {
    const highp float L_white = 4.0;

    return (x * (1.0 + x / (L_white * L_white))) / (1.0 + x);
}

highp float reinhard2(const highp float x) {
    const highp float L_white = 4.0;

    return (x * (1.0 + x / (L_white * L_white))) / (1.0 + x);
}

highp vec3 uncharted2Tonemap(const highp vec3 x) {
//    const highp float A = 0.15;
//    const highp float B = 0.50;
//    const highp float C = 0.10;
//    const highp float D = 0.20;
//    const highp float E = 0.02;
//    const highp float F = 0.30;
//    const highp float W = 11.2;
    const highp float A = 0.22;
    const highp float B = 0.3;
    const highp float C = 0.10;
    const highp float D = 0.20;
    const highp float E = 0.01;
    const highp float F = 0.30;
    const highp float W = 11.2;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

highp vec3 uncharted2(const highp vec3 color) {
    const highp float W = 11.2;
    const highp float exposureBias = 1.0;

    highp vec3 curr = uncharted2Tonemap(exposureBias * color);
    return curr / uncharted2Tonemap(vec3(W, W, W));
}

highp float uncharted2Tonemap(const highp float x) {
//    const highp float A = 0.15;
//    const highp float B = 0.50;
//    const highp float C = 0.10;
//    const highp float D = 0.20;
//    const highp float E = 0.02;
//    const highp float F = 0.30;
//    const highp float W = 11.2;
    const highp float A = 0.22;
    const highp float B = 0.3;
    const highp float C = 0.10;
    const highp float D = 0.20;
    const highp float E = 0.01;
    const highp float F = 0.30;
    const highp float W = 11.2;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

highp float uncharted2(const highp float color) {
    const highp float W = 11.2;
    const highp float exposureBias = 1.0;

    highp float curr = uncharted2Tonemap(exposureBias * color);

    return curr / uncharted2Tonemap(W);
}

// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
highp vec3 unreal(const highp vec3 x) {
    return x / (x + 0.155) * 1.019;
}

highp float unreal(const highp float x) {
    return x / (x + 0.155) * 1.019;
}

#endif // TONEMAP_GLSL
