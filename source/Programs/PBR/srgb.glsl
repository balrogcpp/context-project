// created by Andrey Vasiliev

#ifndef SRGB_GLSL
#define SRGB_GLSL
#define MANUAL_SRGB
#define SRGB_FAST_APPROXIMATION
//#define SRGB_VERY_FAST_APPROXIMATION

mediump vec3 SRGBtoLINEAR(const mediump vec3 srgbIn)
{
#ifdef MANUAL_SRGB
#if defined(SRGB_FAST_APPROXIMATION)
  mediump vec3 linOut = pow(srgbIn.rgb, vec3(2.2, 2.2, 2.2));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  mediump vec3 linOut = srgbIn.xyz * srgbIn.xyz;
#else
  mediump vec3 bLess = step(vec3(0.04045, 0.04045, 0.04045),srgbIn.rgb);
  mediump vec3 linOut = mix(srgbIn.rgb / vec3(12.92, 12.92, 12.92), pow((srgbIn.rgb + vec3(0.055, 0.055, 0.055)) / vec3(1.055, 1.055, 1.055), vec3(2.4, 2.4, 2.4)), bLess);
#endif //SRGB_FAST_APPROXIMATION

  return linOut;

#else //MANUAL_SRGB
  return srgbIn;
#endif //MANUAL_SRGB
}

mediump vec4 SRGBtoLINEAR(const mediump vec4 srgbIn)
{
  return vec4(SRGBtoLINEAR(srgbIn.rgb), srgbIn.a);
}

mediump vec3 LINEARtoSRGB(const mediump vec3 linIn)
{
#ifdef MANUAL_SRGB
#if defined(SRGB_FAST_APPROXIMATION)
  return pow(linIn, vec3(0.45454545454545454545, 0.45454545454545454545, 0.45454545454545454545));
#elif defined(SRGB_VERY_FAST_APPROXIMATION)
  return sqrt(linIn);
#endif

#else
  return linIn;
#endif //  MANUAL_SRGB
}

mediump vec4 LINEARtoSRGB(const mediump vec4 linIn)
{
  return vec4(LINEARtoSRGB(linIn.rgb), linIn.a);
}

mediump vec3 XYZtoACES2065(const mediump vec3 xyz)
{
  return mtx3x3(
  1.0498110175, 0.0000000000, -0.0000974845,
  -0.4959030231,  1.3733130458, 0.0982400361,
  0.0000000000,  0.0000000000, 0.9912520182
  ) * xyz;
}

mediump vec3 ACES2065toACEScg(const mediump vec3 color)
{
  return mtx3x3(
  1.4514393161, -0.2365107469, -0.2149285693,
  -0.0765537733,  1.1762296998, -0.0996759265,
  0.0083161484,  -0.0060324498, 0.9977163014
  ) * color;
}

mediump vec3 ACES2065tosRGB(const mediump vec3 color)
{
  return mtx3x3(
  2.5216494298, -1.1368885542, -0.3849175932,
  -0.2752135512,  1.3697051510, -0.0943924508,
  -0.0159250101,  -0.1478063681, 1.1638058159
  ) * color;
}

mediump vec3 XYZtoRGB(const mediump vec3 xyz)
{
  return mtx3x3(
  3.240812398895283, -0.9692430170086407, 0.055638398436112804,
  -1.5373084456298136,  1.8759663029085742, -0.20400746093241362,
  -0.4985865229069666,  0.04155503085668564, 1.0571295702861434
  ) * xyz;
}

mediump vec3 RGBtoXYZ(const mediump vec3 xyz)
{
  return mtx3x3(
  0.4124108464885388, 0.21264934272065283, 0.019331758429150258,
  0.3575845678529519,  0.7151691357059038,  0.11919485595098397,
  0.18045380393360833, 0.07218152157344333, 0.9503900340503373
  ) * xyz;
}

mediump vec3 XYZtoSRGB(const mediump vec3 xyz)
{
  return mtx3x3(
  2.64725690611828, -1.1935037210389148, -0.4042041831990032,
  -1.6300124092472343, 2.430299920785373, 0.1755837393709873,
  -0.015785698401480384, -0.1465133606819371, 1.1536248638037725
  ) * xyz;
}

mediump vec3 XYZtoACEScg(const mediump vec3 xyz)
{
  return mtx3x3(
  1.5237361745788764, -0.24829099978242145, -0.22573164190143608,
  -0.8240884615592633, 1.732025357889983, 0.06771316251887041,
  0.008243398885150702, -0.005979678038940186, 0.9889882973537895
  ) * xyz;
}

#endif //SRGB_GLSL
