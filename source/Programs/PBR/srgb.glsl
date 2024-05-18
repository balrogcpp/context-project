// created by Andrey Vasiliev

#ifndef GL_ES
    #define SRGB_FAST_APPROXIMATION
#else
    #define SRGB_VERY_FAST_APPROXIMATION
#endif

float toSRGB(float x)
{
    return (x < 0.0031308 ? x * 12.92 : 1.055 * pow(x, 0.41666) - 0.055);
}

float fromSRGB(float x)
{
    return (x <= 0.040449907) ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}

vec3 SRGBtoLINEAR(const vec3 srgb)
{
#if defined(SRGB_FAST_APPROXIMATION)
    return pow(srgb, vec3(2.2, 2.2, 2.2));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
    return srgb * srgb;
#else
    return vec3(fromSRGB(srgb.x), fromSRGB(srgb.y), fromSRGB(srgb.z));
#endif
}

vec3 LINEARtoSRGB(const vec3 col)
{
#if defined(SRGB_FAST_APPROXIMATION)
    return pow(col, vec3(0.45454545, 0.45454545, 0.45454545));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
    return sqrt(col);
#else
    return vec3(toSRGB(col.x), toSRGB(col.y), toSRGB(col.z));
#endif
}
