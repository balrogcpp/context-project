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

attribute vec4 position;
attribute vec2 uv0;

#ifdef FADER_ENABLE
uniform float fadeRange;
#endif
#ifdef WAVER
uniform float uTime;
uniform vec4 uWaveDirection;
#endif

uniform vec3 uViewPos;
uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uTexWorldViewProjMatrixArray[3];

out vec4 lightSpacePosArray[3];
out vec3 vPosition;
out vec2 vUV;
out float depth;
uniform float NearClipDistance;
uniform float FarClipDistance;// !!! might be 0 for infinite view projection.

#ifdef FADER_ENABLE
out float fade;
#endif

void main()
{
  vec4 pos = uModelMatrix * position;
  vPosition = vec3(pos.xyz) / pos.w;
  vec4 mypos = position;

  vUV = uv0;

#ifdef WAVER
  if (vUV.xy == vec2(0.0)) {
    mypos.y += sin(uTime + position.z + position.y + position.x) * uWaveDirection.y;
    mypos.x += sin(uTime + position.z ) * uWaveDirection.x;
  }
#endif

  // Calculate the position of vertex in light space
  lightSpacePosArray[0] = uTexWorldViewProjMatrixArray[0] * mypos;
  lightSpacePosArray[1] = uTexWorldViewProjMatrixArray[1] * mypos;
  lightSpacePosArray[2] = uTexWorldViewProjMatrixArray[2] * mypos;

//   lightSpacePosArray[0] /= lightSpacePosArray[0].w;
//   lightSpacePosArray[1] /= lightSpacePosArray[1].w;
//   lightSpacePosArray[2] /= lightSpacePosArray[2].w;

  gl_Position = uMVPMatrix * mypos;
  depth = gl_Position.z;

#ifdef FADER_ENABLE
  fade = 1.0 - clamp(gl_Position.z / fadeRange, 0.0, 1.0);
#endif
}