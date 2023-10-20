// created by Andrey Vasiliev

#ifndef SRGB_GLSL
#define SRGB_GLSL
#define SRGB_FAST_APPROXIMATION
//#define SRGB_VERY_FAST_APPROXIMATION

vec3 SRGBtoLINEAR(const vec3 linear)
{
#if defined(SRGB_FAST_APPROXIMATION)
  return pow(linear, vec3(2.2, 2.2, 2.2));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  return linear * linear;
#else
  return linear;
#endif
}

vec3 LINEARtoSRGB(const vec3 linear)
{
#if defined(SRGB_FAST_APPROXIMATION)
  return pow(linear, vec3(0.45454545, 0.45454545, 0.45454545));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  return sqrt(linear);
#else
  return linear;
#endif
}

#endif //SRGB_GLSL
