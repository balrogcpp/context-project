// created by Andrey Vasiliev

#ifndef TONEMAP_GLSL
#define TONEMAP_GLSL


// https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color
//----------------------------------------------------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/Relative_luminance
float luminance(const vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

//----------------------------------------------------------------------------------------------------------------------
// https://www.w3.org/TR/AERT/#color-contrast
float luminance2(const vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

//----------------------------------------------------------------------------------------------------------------------
// https://alienryderflex.com/hsp.html
float luminance3(const vec3 color)
{
    return sqrt(dot(color * color, vec3(0.2126, 0.7152, 0.0722)));
}

//----------------------------------------------------------------------------------------------------------------------
float luminance4(const vec3 color)
{
    return sqrt(dot(color * color, vec3(0.2126, 0.7152, 0.0722)));
}

//----------------------------------------------------------------------------------------------------------------------
vec3 expose(const vec3 color, const float exposure)
{
    return vec3(1.0, 1.0, 1.0) - exp(-color * exposure);
}

//----------------------------------------------------------------------------------------------------------------------
float expose(const float color, const float exposure)
{
    return 1.0 - exp(-color * exposure);
}

//----------------------------------------------------------------------------------------------------------------------
vec3 expose2(const vec3 color, const vec3 exposure)
{
    return exposure.x / exp(clamp(color, exposure.y, exposure.z));
}

//----------------------------------------------------------------------------------------------------------------------
float expose2(const float color, const vec3 exposure)
{
    return exposure.x / exp(clamp(color, exposure.y, exposure.z));
}

//----------------------------------------------------------------------------------------------------------------------
vec3 tonemap(const vec3 inColour, const float lum)
{
    const float middle_grey = 0.72;
    const float fudge = 0.001;
    const float L_white = 1.5;

    // From Reinhard et al
    // "Photographic Tone Reproduction for Digital Images"

    // Initial luminence scaling (equation 2)
    vec3 color = inColour * (middle_grey / (fudge + lum));

    // Control white out (equation 4 nom)
    color *= (1.0 + color / L_white);

    // Final mapping (equation 4 denom)
    color /= (1.0 + color);

    return color;
}

// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
//----------------------------------------------------------------------------------------------------------------------
vec3 aces(const vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
float aces(const float x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// Filmic Tonemapping Operators http://filmicworlds.com/blog/filmic-tonemapping-operators/
//----------------------------------------------------------------------------------------------------------------------
vec3 filmic(const vec3 x) {
    vec3 X = max(vec3(0.0, 0.0, 0.0), x - 0.004);
    vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
    return result;
}

//----------------------------------------------------------------------------------------------------------------------
float filmic(const float x) {
    float X = max(0.0, x - 0.004);
    float result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
    return result;
}

// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"
//----------------------------------------------------------------------------------------------------------------------
vec3 lottes(const vec3 x) {
    const vec3 a = vec3(1.6, 1.6, 1.6);
    const vec3 d = vec3(0.977, 0.977, 0.977);
    const vec3 hdrMax = vec3(8.0, 8.0, 8.0);
    const vec3 midIn = vec3(0.18, 0.18, 0.18);
    const vec3 midOut = vec3(0.267, 0.267, 0.267);

    const vec3 b =
        (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
    const vec3 c =
        (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    return pow(x, a) / (pow(x, a * d) * b + c);
}

//----------------------------------------------------------------------------------------------------------------------
float lottes(const float x) {
    const float a = 1.6;
    const float d = 0.977;
    const float hdrMax = 8.0;
    const float midIn = 0.18;
    const float midOut = 0.267;

    const float b =
        (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
    const float c =
        (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    return pow(x, a) / (pow(x, a * d) * b + c);
}

//----------------------------------------------------------------------------------------------------------------------
vec3 reinhard(const vec3 x) {
    return x / (1.0 + x);
}

//----------------------------------------------------------------------------------------------------------------------
float reinhard(const float x) {
    return x / (1.0 + x);
}

//----------------------------------------------------------------------------------------------------------------------
vec3 reinhard2(const vec3 x) {
    const float L_white = 4.0;

    return (x * (1.0 + x / (L_white * L_white))) / (1.0 + x);
}

//----------------------------------------------------------------------------------------------------------------------
float reinhard2(const float x) {
    const float L_white = 4.0;

    return (x * (1.0 + x / (L_white * L_white))) / (1.0 + x);
}

//----------------------------------------------------------------------------------------------------------------------
vec3 uncharted2Tonemap(const vec3 x) {
//    const float A = 0.15;
//    const float B = 0.50;
//    const float C = 0.10;
//    const float D = 0.20;
//    const float E = 0.02;
//    const float F = 0.30;
//    const float W = 11.2;
    const float A = 0.22;
    const float B = 0.3;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.01;
    const float F = 0.30;
    const float W = 11.2;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

//----------------------------------------------------------------------------------------------------------------------
vec3 uncharted2(const vec3 color) {
    const float W = 11.2;
    const float exposureBias = 1.0;

    vec3 curr = uncharted2Tonemap(exposureBias * color);
    return curr / uncharted2Tonemap(vec3(W, W, W));
}

//----------------------------------------------------------------------------------------------------------------------
float uncharted2Tonemap(const float x) {
//    const float A = 0.15;
//    const float B = 0.50;
//    const float C = 0.10;
//    const float D = 0.20;
//    const float E = 0.02;
//    const float F = 0.30;
//    const float W = 11.2;
    const float A = 0.22;
    const float B = 0.3;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.01;
    const float F = 0.30;
    const float W = 11.2;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

//----------------------------------------------------------------------------------------------------------------------
float uncharted2(const float color) {
    const float W = 11.2;
    const float exposureBias = 1.0;

    float curr = uncharted2Tonemap(exposureBias * color);

    return curr / uncharted2Tonemap(W);
}

// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
//----------------------------------------------------------------------------------------------------------------------
vec3 unreal(const vec3 x) {
    return x / (x + 0.155) * 1.019;
}

//----------------------------------------------------------------------------------------------------------------------
float unreal(const float x) {
    return x / (x + 0.155) * 1.019;
}

#endif // TONEMAP_GLSL
