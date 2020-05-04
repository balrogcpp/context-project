#ifndef GL_ES
#define VERSION 120
#version VERSION
#define USE_TEX_LOD
#if VERSION != 120
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#else
#define in varying
#define out varying
#endif
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : require
#endif
#else
#version 100
#extension GL_OES_standard_derivatives : enable
#extension GL_EXT_shader_texture_lod: enable
#define textureCubeLod textureLodEXT
precision highp float;
#define in varying
#define out varying
#endif

#if VERSION != 120
out vec4 gl_FragColor;
#endif


in vec2 oUv0;

uniform float exposure;
uniform vec4 screenSize;
uniform sampler2D SceneSampler;

//#define MANUAL_SRGB
//#define SRGB_FAST_APPROXIMATION
//#define SRGB_SQRT


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

//------------------------------------------------------------------------------
vec4 LINEARtoSRGB(vec4 srgbIn)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_SQRT
  return vec4(sqrt(srgbIn.rgb), srgbIn.a);
#else
  return vec4(pow(srgbIn.rgb, vec3(1.0 / 2.2)), srgbIn.a);
#endif
#else
  return srgbIn;
#endif
}

//-----------------------------------------------------------------------------
vec3 LINEARtoSRGB(vec3 srgbIn)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_SQRT
  return sqrt(srgbIn);
#else
  return pow(srgbIn.rgb, vec3(1.0 / 2.2));
#endif
#else
  return srgbIn;
#endif
}

void main()
{
  vec3 color = texture2D(SceneSampler, oUv0).rgb;

  const float gamma = 2.2;

  // Exposure tone mapping
  vec3 mapped = vec3(1.0) - exp(-color * exposure);
  // Gamma correction
  mapped = pow(mapped, vec3(1.0 / gamma));

  gl_FragColor = vec4(mapped, 1.0);
}