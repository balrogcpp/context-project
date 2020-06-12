/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef GL_ES
#define VERSION 120
#version VERSION
#define USE_TEX_LOD
#if VERSION != 120
#define varying in
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
#if VERSION != 120
out vec4 gl_FragColor;
#endif
#endif
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : require
#endif
#else
#define VERSION 300
#version VERSION es
#extension GL_OES_standard_derivatives : enable
#extension GL_EXT_shader_texture_lod: enable
#define textureCubeLod textureLodEXT
precision highp float;
#if VERSION == 100
#define in varying
#define out varying
#else
#define varying in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
out vec4 gl_FragColor;
#endif
#endif

uniform samplerCube cubemap;

in vec3 TexCoords; // direction vector representing a 3D texture coordinate

#define MANUAL_SRGB
#define SRGB_FAST_APPROXIMATION
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
    gl_FragColor = SRGBtoLINEAR(textureCube(cubemap, TexCoords));
//    gl_FragColor = textureCube(cubemap, TexCoords);
}
