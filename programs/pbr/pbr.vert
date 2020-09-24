//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

// The MIT License
// Copyright (c) 2016-2017 Mohamad Moneimne and Contributors

#ifndef GL_ES
#define VERSION 120
#version VERSION
#define USE_TEX_LOD
#if VERSION != 120
#define attribute in
#define varying out
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#else
#define in attribute
#define out varying
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
#define in attribute
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
#endif
#endif

#define MAX_LIGHTS 5
#define MAX_SHADOWS 1
#define HAS_UV

#ifndef VERTEX_COMPRESSION
in vec4 position;
in vec2 uv0;
#else
in vec2 vertex; // VES_POSITION
in float uv0; // VES_TEXTURE_COORDINATES0
uniform mat4 posIndexToObjectSpace;
uniform float baseUVScale;
#endif

uniform mat4 uMVPMatrix;

#ifndef SHADOWCASTER_ALPHA
in vec4 colour;
uniform mat4 uModelMatrix;
uniform vec3 uCameraPosition;
#ifdef PAGED_GEOMETRY
uniform float fadeRange;
#endif
uniform float uTime;
out vec4 vUV0;
#ifdef HAS_NORMALS
in vec4 normal;
#endif
#ifdef HAS_TANGENTS
in vec4 tangent;
#endif
#ifdef SHADOWRECEIVER
uniform float uLightCount;
uniform float uLightCastsShadowsArray[MAX_LIGHTS];
uniform mat4 uTexWorldViewProjMatrixArray[3 * MAX_SHADOWS];
out vec4 lightSpacePosArray[3 * MAX_SHADOWS];
#endif
out vec3 vPosition;
out vec3 vColor;
#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
out mat3 vTBN;
#else
out vec3 vNormal;
#endif
#endif
#ifdef REFLECTION
out vec4 projectionCoord;
#endif
#else //SHADOWCASTER
out vec2 vUV0;
#endif

void main()
{
#ifndef VERTEX_COMPRESSION
  vec4 new_position = position;
#else
  vec4 new_position = posIndexToObjectSpace * vec4(vertex, uv0, 1.0);
#define uv0 _uv0
  vec2 uv0 = vec2(vertex.x * baseUVScale, 1.0 - (vertex.y * baseUVScale));
#endif

  vUV0.xy = uv0;

#ifdef PAGED_GEOMETRY
if (uv0.y == 0.0)
{
  float kradius = 0.25;
  float kheigh = 0.25;
  float kx = 1.0;
  float ky = 1.0;
  new_position.y += sin(uTime + new_position.z + new_position.y + new_position.x) * kradius * kradius * ky;
  new_position.x += sin(uTime + new_position.z) * kheigh * kheigh * kx;
}
#endif
#ifndef SHADOWCASTER
  vColor = colour.rgb;
  vec4 model_position = uModelMatrix * new_position;
  vPosition = model_position.xyz / model_position.w;
#ifdef PAGED_GEOMETRY
  float dist = distance(uCameraPosition.xz, vPosition.xz);
  vUV0.w = 2.0f - (2.0f * dist * fadeRange);
  float offset = (2.0f * dist * fadeRange) - 1.0f;
  new_position.y -= 2.0f * clamp(offset, 0, 1);
#endif
#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
  vec3 n = normalize(vec3(uModelMatrix * vec4(normal.xyz, 0.0)));
  vec3 t = normalize(vec3(uModelMatrix * vec4(tangent.xyz, 0.0)));
  vec3 b = cross(n, t) * tangent.w;
  vTBN = mat3(t, b, n);
#else // HAS_TANGENTS != 1
  vNormal = normalize(vec3(uModelMatrix * vec4(normal.xyz, 0.0)));
#endif
#else
#endif

  gl_Position = uMVPMatrix * new_position;
  vUV0.z = gl_Position.z;
#ifdef REFLECTION
  const mat4 scalemat = mat4(0.5, 0.0, 0.0, 0.0,
                        0.0, 0.5, 0.0, 0.0,
                        0.0, 0.0, 0.5, 0.0,
                        0.5, 0.5, 0.5, 1.0);
  projectionCoord = scalemat * gl_Position;
#endif
#ifdef SHADOWRECEIVER
  // Calculate the position of vertex in light space
  int j = 0;
  for (int i = 0; i < int(uLightCount);  i++) {
    if (uLightCastsShadowsArray[i] == 1.0) {
      lightSpacePosArray[j] = uTexWorldViewProjMatrixArray[j] * new_position;
      lightSpacePosArray[j + 1] = uTexWorldViewProjMatrixArray[j + 1] * new_position;
      lightSpacePosArray[j + 2] = uTexWorldViewProjMatrixArray[j + 2] * new_position;
    }
    j += 3;
  }
#endif

#else //SHADOWCASTER
#ifdef SHADOWCASTER_ALPHA
  vUV0.xy = uv0;
#endif
  gl_Position = uMVPMatrix * new_position;
#endif //SHADOWCASTER
}