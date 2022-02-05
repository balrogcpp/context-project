// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef SRGB_GLSL
#define SRGB_GLSL

#define MANUAL_SRGB

#define SRGB_FAST_APPROXIMATION
#define SRGB_HDR
#define SRGB_SQRT

//----------------------------------------------------------------------------------------------------------------------
vec4 SRGBtoLINEAR(vec4 srgbIn)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_FAST_APPROXIMATION
#ifdef SRGB_SQRT
  vec3 linOut = srgbIn.rgb * srgbIn.rgb;
#else
  vec3 linOut = pow(srgbIn.rgb, vec3(2.2));
#endif
#else //SRGB_FAST_APPROXIMATION
  vec3 bLess = step(vec3(0.04045),srgbIn.rgb);
  vec3 linOut = mix(srgbIn.rgb/vec3(12.92), pow((srgbIn.rgb+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
#endif //SRGB_FAST_APPROXIMATION
  return vec4(linOut,srgbIn.w);
#else //MANUAL_SRGB
  return srgbIn;
#endif //MANUAL_SRGB
}

//----------------------------------------------------------------------------------------------------------------------
vec3 SRGBtoLINEAR(vec3 srgbIn)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_FAST_APPROXIMATION
#ifdef SRGB_SQRT
  vec3 linOut = srgbIn.xyz * srgbIn.xyz;
#else
  vec3 linOut = pow(srgbIn.rgb, vec3(2.2));
#endif
  #else //SRGB_FAST_APPROXIMATION
  vec3 bLess = step(vec3(0.04045),srgbIn.rgb);
  vec3 linOut = mix( srgbIn.rgb/vec3(12.92), pow((srgbIn.rgb+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
#endif //SRGB_FAST_APPROXIMATION
  return linOut;
#else //MANUAL_SRGB
  return srgbIn;
#endif //MANUAL_SRGB
}

//----------------------------------------------------------------------------------------------------------------------
vec4 LINEARtoSRGB(vec4 srgbIn, float exposure)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_HDR
  srgbIn.rgb = vec3(1.0) - exp(-srgbIn.rgb * exposure);
#endif
#ifdef SRGB_SQRT
  return vec4(sqrt(srgbIn.rgb), srgbIn.a);
#else
  return vec4(pow(srgbIn.rgb, vec3(1.0 / 2.2)), srgbIn.a);
#endif
#else
  return srgbIn;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
vec3 LINEARtoSRGB(vec3 srgbIn, float exposure)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_HDR
  srgbIn.rgb = vec3(1.0) - exp(-srgbIn.rgb * exposure);
#endif
#ifdef SRGB_SQRT
  return sqrt(srgbIn);
#else
  return pow(srgbIn.rgb, vec3(1.0 / 2.2));
#endif
#else
  return srgbIn;
#endif
}

#endif //SRGB_GLSL
