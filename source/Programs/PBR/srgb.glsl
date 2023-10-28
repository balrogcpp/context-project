// created by Andrey Vasiliev

#ifndef SRGB_GLSL
#define SRGB_GLSL
#define SRGB_FAST_APPROXIMATION
//#define SRGB_VERY_FAST_APPROXIMATION

float toSRGB(float x)
{
    return (x < 0.0031308 ? x * 12.92 : 1.055 * pow(x, 0.41666 ) - 0.055);
}

float fromSRGB(float x)
{
    return (x <= 0.040449907) ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}

vec3 SRGBtoLINEAR(vec3 srgb)
{
#if defined(SRGB_FAST_APPROXIMATION)
    return pow(srgb, vec3(2.2, 2.2, 2.2));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
    return srgb * srgb;
#else
    vec3 bLess = step(vec3(0.04045, 0.04045, 0.04045), srgb);
    return mix(srgb / 12.92, pow(srgb + 0.055/1.055, vec3(2.4, 2.4, 2.4)), bLess);
#endif
}

vec3 LINEARtoSRGB(vec3 linear)
{
#if defined(SRGB_FAST_APPROXIMATION)
    return pow(linear, vec3(0.45454545, 0.45454545, 0.45454545));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
    return sqrt(linear);
#else
    return vec3(toSRGB(linear.x), toSRGB(linear.y), toSRGB(linear.z));
#endif
}

#endif //SRGB_GLSL
