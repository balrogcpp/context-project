// created by Andrey Vasiliev

#define SRGB_FAST_APPROXIMATION

vec3 SRGBtoLINEAR(const vec3 srgb)
{
#if defined(SRGB_FAST_APPROXIMATION)
    return pow(srgb, vec3(2.2, 2.2, 2.2));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
    return srgb * srgb;
#else
    vec3 bLess = step(vec3(0.040449907, 0.040449907, 0.040449907), srgb);
    return mix(srgb / 12.92, pow((srgb + 0.055) / 1.055, vec3(2.4, 2.4, 2.4)), bLess);
#endif
}

vec3 LINEARtoSRGB(const vec3 lin)
{
#if defined(SRGB_FAST_APPROXIMATION)
    return pow(lin, vec3(0.45454545, 0.45454545, 0.45454545));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
    return sqrt(lin);
#else
    vec3 bLess = step(vec3(0.0031308, 0.0031308, 0.0031308), lin);
    return mix(lin * 12.92, 1.055 * pow(lin, vec3(0.41666, 0.41666, 0.41666)) - 0.055, bLess);
#endif
}
