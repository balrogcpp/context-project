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
#define attribute in
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
#define attribute in
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

in vec2 oUv0;

uniform sampler2D sNormal;
uniform sampler2D sPosition;
uniform sampler2D AttenuationSampler;

uniform float cRange; // the three(four) artistic parameters
uniform float cBias;
uniform float cAverager;
uniform float cMinimumCrease;
uniform float cKernelSize; // Bias for the kernel size, Hack for the fixed size 11x11 stipple kernel
uniform vec4 cViewportSize;

void main()
{
  // get the view space position and normal of the fragment
  vec3 fragmentPosition = texture2D(sPosition, oUv0).xyz;
  vec3 fragmentNormal = texture2D(sNormal, oUv0).xyz;

  float totalGI = 0.0f;

  const int stippleSize = 11; // must be odd
  for (int i = 0; i < (stippleSize + 1) / 2; i++)
  {
    vec2 diagonalStart = vec2(-(stippleSize - 1.0) / 2.0, 0) + i;
    for(int j = 0; j < (stippleSize + 1) / 2; j++)
    {
      vec2 sampleOffset = diagonalStart + vec2(j, -j);

      vec2 sampleUV = oUv0 + (sampleOffset * cViewportSize.zw * cKernelSize);
      vec3 samplePos = texture2D(sPosition, sampleUV).xyz;

      vec3 toCenter = samplePos - fragmentPosition;
      float distance = length(toCenter);

      toCenter = normalize(toCenter);
      float centerContrib = clamp((dot(toCenter, fragmentNormal) - cMinimumCrease) * cBias, 0.0, 1.0);
      float rangeAttenuation = 1.0f - clamp(distance / cRange, 0.0, 1.0);

      totalGI += centerContrib * rangeAttenuation;
    }
  }

  totalGI /= cAverager;

  vec4 attenuation = texture2D(AttenuationSampler, oUv0);
  gl_FragColor = vec4(attenuation.rgb, 1.0 - totalGI);
}