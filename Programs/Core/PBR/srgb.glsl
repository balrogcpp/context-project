// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef SRGB_GLSL
#define SRGB_GLSL

#define MANUAL_SRGB
#define SRGB_FAST_APPROXIMATION
#define SRGB_HDR
#define SRGB_SQRT

//----------------------------------------------------------------------------------------------------------------------
vec3 expose(vec3 color, float exposure) {
  //return vec3(2.0) / (vec3(1.0) + exp(-exposure * color)) - vec3(1.0);
  return vec3(1.0) - exp(-color.rgb * exposure);
}

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
  srgbIn.rgb = expose(srgbIn.rgb, exposure);
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
  srgbIn.rgb = expose(srgbIn.rgb, exposure);
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

vec3 XYZ_to_ACES2065_1(vec3 color)
{
  return mat3(
  1.0498110175, 0.0000000000, -0.0000974845,
  -0.4959030231,  1.3733130458, 0.0982400361,
  0.0000000000,  0.0000000000, 0.9912520182
  ) * color;
}

vec3 ACES2065_1_to_ACEScg(vec3 color)
{
  return mat3(
  1.4514393161, -0.2365107469, -0.2149285693,
  -0.0765537733,  1.1762296998, -0.0996759265,
  0.0083161484,  -0.0060324498, 0.9977163014
  ) * color;
}

vec3 ACES2065_1_to_sRGB(vec3 color)
{
  return mat3(
  2.5216494298, -1.1368885542, -0.3849175932,
  -0.2752135512,  1.3697051510, -0.0943924508,
  -0.0159250101,  -0.1478063681, 1.1638058159
  ) * color;
}

#endif //SRGB_GLSL
