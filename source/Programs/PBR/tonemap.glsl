// created by Andrey Vasiliev

#ifndef TONEMAP_GLSL
#define TONEMAP_GLSL


// https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color
//----------------------------------------------------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/Relative_luminance
mediump float luminance(const mediump vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}


//----------------------------------------------------------------------------------------------------------------------
// https://www.w3.org/TR/AERT/#color-contrast
mediump float luminance2(const mediump vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}


//----------------------------------------------------------------------------------------------------------------------
// https://alienryderflex.com/hsp.html
mediump float luminance3(const mediump vec3 color)
{
    return sqrt(dot(color * color, vec3(0.299, 0.587, 0.114)));
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 expose(const mediump vec3 color, const mediump float exposure)
{
    return vec3(1.0, 1.0, 1.0) - exp(-color.rgb * exposure);
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 tonemap(const mediump vec3 inColour, const mediump float lum)
{
    #define MIDDLE_GREY 0.72
    #define FUDGE 0.001
    #define L_WHITE 1.5

    // From Reinhard et al
    // "Photographic Tone Reproduction for Digital Images"

    // Initial luminence scaling (equation 2)
    mediump vec3 color = inColour * (MIDDLE_GREY / (FUDGE + lum));

    // Control white out (equation 4 nom)
    color *= (1.0 + color / L_WHITE);

    // Final mapping (equation 4 denom)
    color /= (1.0 + color);

    return color;
}


// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
mediump vec3 aces(const mediump vec3 x) {
    const mediump float a = 2.51;
    const mediump float b = 0.03;
    const mediump float c = 2.43;
    const mediump float d = 0.59;
    const mediump float e = 0.14;

    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

mediump float aces(const mediump float x) {
    const mediump float a = 2.51;
    const mediump float b = 0.03;
    const mediump float c = 2.43;
    const mediump float d = 0.59;
    const mediump float e = 0.14;

    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// Filmic Tonemapping Operators http://filmicworlds.com/blog/filmic-tonemapping-operators/
mediump vec3 filmic(const mediump vec3 x) {
    mediump vec3 X = max(vec3(0.0, 0.0, 0.0), x - 0.004);
    mediump vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
    return pow(result, vec3(2.2, 2.2, 2.2));
}

mediump float filmic(const mediump float x) {
    mediump float X = max(0.0, x - 0.004);
    mediump float result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
    return pow(result, 2.2);
}

// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"
mediump vec3 lottes(mediump vec3 x) {
    const mediump vec3 a = vec3(1.6);
    const mediump vec3 d = vec3(0.977);
    const mediump vec3 hdrMax = vec3(8.0);
    const mediump vec3 midIn = vec3(0.18);
    const mediump vec3 midOut = vec3(0.267);

    const mediump vec3 b =
        (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
    const mediump vec3 c =
        (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    return pow(x, a) / (pow(x, a * d) * b + c);
}

mediump float lottes(const mediump float x) {
    const mediump float a = 1.6;
    const mediump float d = 0.977;
    const mediump float hdrMax = 8.0;
    const mediump float midIn = 0.18;
    const mediump float midOut = 0.267;

    const mediump float b =
        (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
    const mediump float c =
        (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    return pow(x, a) / (pow(x, a * d) * b + c);
}

mediump vec3 reinhard(const mediump vec3 x) {
    return x / (1.0 + x);
}

mediump float reinhard(const mediump float x) {
    return x / (1.0 + x);
}

mediump vec3 reinhard2(const mediump vec3 x) {
    const mediump float L_white = 4.0;

    return (x * (1.0 + x / (L_white * L_white))) / (1.0 + x);
}

mediump float reinhard2(const mediump float x) {
    const mediump float L_white = 4.0;

    return (x * (1.0 + x / (L_white * L_white))) / (1.0 + x);
}


mediump vec3 uncharted2Tonemap(mediump vec3 x) {
    const mediump float A = 0.15;
    const mediump float B = 0.50;
    const mediump float C = 0.10;
    const mediump float D = 0.20;
    const mediump float E = 0.02;
    const mediump float F = 0.30;
    const mediump float W = 11.2;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

mediump vec3 uncharted2(const mediump vec3 color) {
    const mediump float W = 11.2;
    const mediump float exposureBias = 2.0;

    mediump vec3 curr = uncharted2Tonemap(exposureBias * color);
    mediump vec3 whiteScale = 1.0 / uncharted2Tonemap(vec3(W));
    return curr * whiteScale;
}

mediump float uncharted2Tonemap(const mediump float x) {
    const mediump float A = 0.15;
    const mediump float B = 0.50;
    const mediump float C = 0.10;
    const mediump float D = 0.20;
    const mediump float E = 0.02;
    const mediump float F = 0.30;
    const mediump float W = 11.2;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

mediump float uncharted2(const mediump float color) {
    const mediump float W = 11.2;
    const mediump float exposureBias = 2.0;

    mediump float curr = uncharted2Tonemap(exposureBias * color);
    mediump float whiteScale = 1.0 / uncharted2Tonemap(W);
    return curr * whiteScale;
}

// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
mediump vec3 unreal(const mediump vec3 x) {
    return x / (x + 0.155) * 1.019;
}

mediump float unreal(const mediump float x) {
    return x / (x + 0.155) * 1.019;
}

#endif // TONEMAP_GLSL
