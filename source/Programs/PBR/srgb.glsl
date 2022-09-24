/// created by Andrey Vasiliev

#ifndef SRGB_GLSL
#define SRGB_GLSL

#define MANUAL_SRGB
#define SRGB_FAST_APPROXIMATION
#define SRGB_HDR
//#define SRGB_SQRT

//----------------------------------------------------------------------------------------------------------------------
vec3 expose(const vec3 color, const float exposure)
{
  return vec3(1.0) - exp(-color.rgb * exposure);
}

//----------------------------------------------------------------------------------------------------------------------
vec4 expose(const vec4 color, const float exposure)
{
  return vec4(1.0) - vec4(exp(-color.rgb * exposure), color.a);
}

//----------------------------------------------------------------------------------------------------------------------
vec4 SRGBtoLINEAR(const vec4 srgbIn)
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
vec3 SRGBtoLINEAR(const vec3 srgbIn)
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
vec4 LINEARtoSRGB(const vec4 linIn, const float exposure)
{
  vec4 srgbOut;
#ifdef MANUAL_SRGB
#ifdef SRGB_HDR
  srgbOut = expose(linIn, exposure);
#endif
#ifdef SRGB_SQRT
  return vec4(sqrt(srgbOut.rgb), linIn.a);
#else
  return vec4(pow(srgbOut.rgb, vec3(1.0 / 2.2)), linIn.a);
#endif
#else
  return srgbOut;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
vec3 LINEARtoSRGB(const vec3 linIn, const float exposure)
{
  vec3 srgbOut;
#ifdef MANUAL_SRGB
#ifdef SRGB_HDR
  srgbOut = expose(linIn, exposure);
#endif
#ifdef SRGB_SQRT
  return sqrt(srgbOut);
#else
  return pow(srgbOut, vec3(1.0 / 2.2));
#endif
#else
  return srgbOut;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
vec4 LINEARtoSRGB(const vec4 linIn)
{
  vec4 srgbOut;
#ifdef MANUAL_SRGB
#ifdef SRGB_HDR
  srgbOut = expose(linIn, 1.0);
#endif
#ifdef SRGB_SQRT
  return vec4(sqrt(srgbOut.rgb), linIn.a);
#else
  return vec4(pow(srgbOut.rgb, vec3(1.0 / 2.2)), linIn.a);
#endif
#else
  return srgbOut;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
vec3 LINEARtoSRGB(const vec3 linIn)
{
  vec3 srgbOut;
#ifdef MANUAL_SRGB
#ifdef SRGB_HDR
  srgbOut = expose(linIn, 1.0);
#endif
#ifdef SRGB_SQRT
  return sqrt(srgbOut);
#else
  return pow(srgbOut, vec3(1.0 / 2.2));
#endif
#else
  return srgbOut;
#endif
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
  3.24096994, -0.96924364, 0.55630080,
  -1.53738318,  1.8759675, -0.20397696,
  -0.49861076,  0.04155506, 1.05697151
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
