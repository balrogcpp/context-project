//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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
#else
#define VERSION 100
#version VERSION
#endif
#include "header.vert"

#ifdef SHADOWCASTER_ALPHA
#define HAS_UV
#endif

#ifndef VERTEX_COMPRESSION
in vec3 position;
#ifdef HAS_UV
in vec2 uv0;
#endif
#else
in vec2 vertex;
in float uv0;
uniform mat4 posIndexToObjectSpace;
uniform float baseUVScale;
#endif

uniform mat4 uMVPMatrix;

#ifndef SHADOWCASTER
#ifdef HAS_COLOURS
in vec3 colour;
#endif //HAS_COLOURS
uniform mat4 uModelMatrix;
uniform vec3 uCameraPosition;
#ifdef PAGED_GEOMETRY
#define HAS_UV
uniform float uFadeRange;
uniform float uWindRange;
#endif //PAGED_GEOMETRY
uniform float uTime;
out vec2 vUV0;
out float vDepth;
out float vAlpha;
#ifdef HAS_NORMALS
in vec3 normal;
#endif //HAS_NORMALS
#ifdef HAS_TANGENTS
in vec4 tangent;
#endif //HAS_TANGENTS
#ifdef SHADOWRECEIVER
uniform int uShadowTextureCount;
uniform mat4 uTexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
out vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif //SHADOWRECEIVER
out vec3 vPosition;
#ifdef HAS_COLOURS
out vec3 vColor;
#endif //HAS_COLOURS
#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
out mat3 vTBN;
#else //!HAS_TANGENTS
out vec3 vNormal;
#endif //HAS_TANGENTS
#endif //HAS_NORMALS
#ifdef HAS_REFLECTION
out vec4 projectionCoord;
#endif //HAS_REFLECTION

#else //SHADOWCASTER
out vec2 vUV0;
#endif

#ifdef HAS_REFLECTION
vec4 GetProjectionCoord(vec4 position) {
  const mat4 scalemat = mat4(0.5, 0.0, 0.0, 0.0,
                            0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0,
                            0.5, 0.5, 0.5, 1.0);

  return scalemat * position;
}
#endif

#ifdef PAGED_GEOMETRY
float hash( float n ) {
  return fract(sin(n)*43758.5453);
}

float noise(vec2 x) {
  vec2 p = floor(x);
  vec2 f = fract(x);
  f = f*f*(3.0-2.0*f);
  float n = p.x + p.y*57.0;
  float res = mix(mix( hash(n+  0.0), hash(n+  1.0),f.x), mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y);
  return res;
}

float fbm(vec2 p) {
  float f = 0.0;
  f += 0.50000*noise(p); p = p*2.02;
//  f += 0.25000*noise( p ); p = p*2.03;
//  f += 0.12500*noise( p ); p = p*2.01;
//  f += 0.06250*noise( p ); p = p*2.04;
//  f += 0.03125*noise(p);
  return f/0.984375;
}

vec4 ApplyWaveAnimation(vec4 position, float time, float frequency, vec4 direction) {
  float offset = sin(uTime + position.x * frequency);
  float n = fbm(position.xy * time * 0.2) * 4.0 - 2.0;
//  return position + offset * direction + n * 0.01;
  return position + n * direction;
}
#endif

void main()
{
#ifndef VERTEX_COMPRESSION
  vec4 new_position = vec4(position, 1.0);
#else
  vec4 new_position = posIndexToObjectSpace * vec4(vertex, uv0.xy, 1.0);
  vec2 uv0 = vec2(vertex.x * baseUVScale, 1.0 - (vertex.y * baseUVScale));
#endif

#ifdef HAS_UV
  vUV0.xy = uv0.xy;
#else
  vUV0.xy = vec2(0.0);
#endif

#ifndef SHADOWCASTER
#ifdef HAS_COLOURS
  vColor = colour.rgb;
#endif

  vec4 model_position = uModelMatrix * new_position;
  vPosition = model_position.xyz / model_position.w;

#ifdef PAGED_GEOMETRY
  float dist = distance(uCameraPosition.xyz, vPosition.xyz);
  vAlpha = (dist < uFadeRange) ? 1.0 : 0.0;

  if (uv0.y < 0.9 && dist < uWindRange) {
    new_position = ApplyWaveAnimation(new_position, uTime, 1.0, vec4(0.25, 0.1, 0.25, 0.0));
  }
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
  vDepth = gl_Position.z;

#ifdef SHADOWRECEIVER
  // Calculate the position of vertex in light space
  for (int i = 0; i < uShadowTextureCount; i++) {
      lightSpacePosArray[i] = uTexWorldViewProjMatrixArray[i] * new_position;
  }
#endif

#ifdef HAS_REFLECTION
  projectionCoord = GetProjectionCoord(gl_Position);
#endif

#else //SHADOWCASTER
  gl_Position = uMVPMatrix * new_position;
#endif //SHADOWCASTER
}
