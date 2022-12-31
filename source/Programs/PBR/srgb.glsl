// created by Andrey Vasiliev

#ifndef SRGB_GLSL
#define SRGB_GLSL


#define MANUAL_SRGB

//#define SRGB_HDR
#ifdef GL_ES
  #define SRGB_VERY_FAST_APPROXIMATION
#else
  #define SRGB_FAST_APPROXIMATION
#endif


// https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color
//----------------------------------------------------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/Relative_luminance
float luminance(const vec3 color)
{
  return dot(color, vec3(0.2126, 0.7152, 0.0722));
}


//----------------------------------------------------------------------------------------------------------------------
float luminance(const vec4 color)
{
  return dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
}


//----------------------------------------------------------------------------------------------------------------------
// https://www.w3.org/TR/AERT/#color-contrast
float luminance2(const vec3 color)
{
  return dot(color, vec3(0.299, 0.587, 0.114));
}


//----------------------------------------------------------------------------------------------------------------------
float luminance2(const vec4 color)
{
  return dot(color.rgb, vec3(0.299, 0.587, 0.114));
}


//----------------------------------------------------------------------------------------------------------------------
// https://alienryderflex.com/hsp.html
float luminance3(const vec3 color)
{
  return sqrt(dot(color * color, vec3(0.299, 0.587, 0.114)));
}


//----------------------------------------------------------------------------------------------------------------------
float luminance3(const vec4 color)
{
  return sqrt(dot(color.rgb * color.rgb, vec3(0.299, 0.587, 0.114)));
}


//----------------------------------------------------------------------------------------------------------------------
vec3 expose(const vec3 color, const float exposure)
{
  return vec3(1.0) - exp(-color.rgb * exposure);
}


//----------------------------------------------------------------------------------------------------------------------
vec4 expose(const vec4 color, const float exposure)
{
  return vec4(expose(color.rgb, exposure), color.a);
}


//----------------------------------------------------------------------------------------------------------------------
vec3 tonemap(const vec3 inColour, const float lum)
{
  #define MIDDLE_GREY 0.72
  #define FUDGE 0.001
  #define L_WHITE 1.5

  // From Reinhard et al
  // "Photographic Tone Reproduction for Digital Images"

  // Initial luminence scaling (equation 2)
  vec3 color = inColour * (MIDDLE_GREY / (FUDGE + lum));

  // Control white out (equation 4 nom)
  color *= (1.0 + color / L_WHITE);

  // Final mapping (equation 4 denom)
  color /= (1.0 + color);

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec4 SRGBtoLINEAR(const vec4 srgbIn)
{
#ifdef MANUAL_SRGB

#if defined(SRGB_FAST_APPROXIMATION)
    vec3 linOut = pow(srgbIn.rgb, vec3(2.2));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  vec3 linOut = srgbIn.rgb * srgbIn.rgb;
#else
  vec3 bLess = step(vec3(0.04045),srgbIn.rgb);
  vec3 linOut = mix(srgbIn.rgb/vec3(12.92), pow((srgbIn.rgb+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess);
#endif //SRGB_FAST_APPROXIMATION

  return vec4(linOut,srgbIn.a);

#else //MANUAL_SRGB
  return srgbIn;
#endif //MANUAL_SRGB
}


//----------------------------------------------------------------------------------------------------------------------
vec3 SRGBtoLINEAR(const vec3 srgbIn)
{
#ifdef MANUAL_SRGB

#if defined(SRGB_FAST_APPROXIMATION)
  vec3 linOut = pow(srgbIn.rgb, vec3(2.2));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  vec3 linOut = srgbIn.xyz * srgbIn.xyz;
#else
  vec3 bLess = step(vec3(0.04045),srgbIn.rgb);
  vec3 linOut = mix(srgbIn.rgb/vec3(12.92), pow((srgbIn.rgb+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess);
#endif //SRGB_FAST_APPROXIMATION

  return linOut;

#else //MANUAL_SRGB
  return srgbIn;
#endif //MANUAL_SRGB
}


//----------------------------------------------------------------------------------------------------------------------
vec4 LINEARtoSRGB(const vec4 linIn, const float exposure)
{
#ifdef MANUAL_SRGB

#ifdef SRGB_HDR
  vec4 srgbOut = expose(linIn, exposure);
#else
  vec4 srgbOut = linIn;
#endif

#if defined(SRGB_FAST_APPROXIMATION)
  return vec4(pow(srgbOut.rgb, vec3(1.0 / 2.2)), linIn.a);
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  return vec4(sqrt(srgbOut.rgb), linIn.a);
#endif

  return srgbOut;
#endif //  MANUAL_SRGB
}


//----------------------------------------------------------------------------------------------------------------------
vec3 LINEARtoSRGB(const vec3 linIn, const float exposure)
{
#ifdef MANUAL_SRGB

#ifdef SRGB_HDR
  vec3 srgbOut = expose(linIn, exposure);
#else
  vec3 srgbOut = linIn;
#endif

#if defined(SRGB_FAST_APPROXIMATION)
  return pow(srgbOut, vec3(1.0 / 2.2));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  return sqrt(srgbOut);
#endif
  
  return srgbOut;
#endif //  MANUAL_SRGB
}


//----------------------------------------------------------------------------------------------------------------------
vec4 LINEARtoSRGB(const vec4 linIn)
{
#ifdef MANUAL_SRGB

#if defined(SRGB_FAST_APPROXIMATION)
  return vec4(pow(linIn.rgb, vec3(1.0 / 2.2)), linIn.a);
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  return vec4(sqrt(linIn.rgb), linIn.a);
#endif

#else
  return linIn;
#endif //  MANUAL_SRGB
}


//----------------------------------------------------------------------------------------------------------------------
vec3 LINEARtoSRGB(const vec3 linIn)
{
#ifdef MANUAL_SRGB

#if defined(SRGB_FAST_APPROXIMATION)
  return pow(linIn, vec3(1.0 / 2.2));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  return sqrt(linIn);
#endif

#else
  return linIn;
#endif //  MANUAL_SRGB
}


//----------------------------------------------------------------------------------------------------------------------
vec3 XYZtoACES2065(const vec3 xyz)
{
  return mat3(
  1.0498110175, 0.0000000000, -0.0000974845,
  -0.4959030231,  1.3733130458, 0.0982400361,
  0.0000000000,  0.0000000000, 0.9912520182
  ) * xyz;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 ACES2065toACEScg(const vec3 color)
{
  return mat3(
  1.4514393161, -0.2365107469, -0.2149285693,
  -0.0765537733,  1.1762296998, -0.0996759265,
  0.0083161484,  -0.0060324498, 0.9977163014
  ) * color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 ACES2065tosRGB(const vec3 color)
{
  return mat3(
  2.5216494298, -1.1368885542, -0.3849175932,
  -0.2752135512,  1.3697051510, -0.0943924508,
  -0.0159250101,  -0.1478063681, 1.1638058159
  ) * color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 XYZtoRGB(const vec3 xyz)
{
  return mat3(
  3.240812398895283, -0.9692430170086407, 0.055638398436112804,
  -1.5373084456298136,  1.8759663029085742, -0.20400746093241362,
  -0.4985865229069666,  0.04155503085668564, 1.0571295702861434
  ) * xyz;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 RGBtoXYZ(const vec3 xyz)
{
  return mat3(
  0.4124108464885388, 0.21264934272065283, 0.019331758429150258,
  0.3575845678529519,  0.7151691357059038,  0.11919485595098397,
  0.18045380393360833, 0.07218152157344333, 0.9503900340503373
  ) * xyz;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 XYZtoSRGB(const vec3 xyz)
{
  return mat3(
  2.64725690611828, -1.1935037210389148, -0.4042041831990032,
  -1.6300124092472343, 2.430299920785373, 0.1755837393709873,
  -0.015785698401480384, -0.1465133606819371, 1.1536248638037725
  ) * xyz;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 XYZtoACEScg(const vec3 xyz)
{
  return mat3(
  1.5237361745788764, -0.24829099978242145, -0.22573164190143608,
  -0.8240884615592633, 1.732025357889983, 0.06771316251887041,
  0.008243398885150702, -0.005979678038940186, 0.9889882973537895
  ) * xyz;
}


#endif //SRGB_GLSL
