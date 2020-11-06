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
#else
#define VERSION 100
#version VERSION
#endif
#include "header.vert"

#define HAS_UV

#ifndef VERTEX_COMPRESSION
in vec3 position;
in vec2 uv0;
#else
in vec2 vertex;
in float uv0;
uniform mat4 posIndexToObjectSpace;
uniform float baseUVScale;
#endif

uniform mat4 uMVPMatrix;

#ifndef SHADOWCASTER_ALPHA
#ifdef HAS_COLOURS
in vec3 colour;
#endif
uniform mat4 uModelMatrix;
uniform vec3 uCameraPosition;
#ifdef PAGED_GEOMETRY
uniform float fadeRange;
uniform float windRange;
#endif
uniform float uTime;
out vec2 vUV0;
out float vDepth;
out float vAlpha;
#ifdef HAS_NORMALS
in vec3 normal;
#endif
#ifdef HAS_TANGENTS
in vec4 tangent;
#endif
#ifdef SHADOWRECEIVER
uniform float uLightCount;
uniform float uLightCastsShadowsArray[MAX_LIGHTS];
uniform mat4 uTexWorldViewProjMatrixArray[MAX_SHADOWS];
out vec4 lightSpacePosArray[MAX_SHADOWS];
#endif
out vec3 vPosition;
#ifdef HAS_COLOURS
out vec3 vColor;
#endif
#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
out mat3 vTBN;
#else
out vec3 vNormal;
#endif
#endif
#ifdef HAS_REFLECTION
out vec4 projectionCoord;
#endif
#else //SHADOWCASTER
out vec2 vUV0;
#endif

void main()
{
#ifndef VERTEX_COMPRESSION
  vec4 new_position = vec4(position, 1.0);
#else
  vec4 new_position = posIndexToObjectSpace * vec4(vertex, uv0.xy, 1.0);
  vec2 uv0 = vec2(vertex.x * baseUVScale, 1.0 - (vertex.y * baseUVScale));
#endif

  vUV0.xy = uv0.xy;

#ifndef SHADOWCASTER
#ifdef HAS_COLOURS
  vColor = colour.rgb;
#endif

  vec4 model_position = uModelMatrix * new_position;
  vPosition = model_position.xyz / model_position.w;
#ifdef PAGED_GEOMETRY
  float dist = distance(uCameraPosition.xz, vPosition.xz);
  if (uv0.xy == vec2(0.0) && dist < windRange) {
    const float frequency = 4.0;
    const vec4 direction = vec4(0.2, 0.2, 0, 0);
    float offset = sin(uTime + new_position.x * frequency);
    new_position += direction * offset;
  }

  vAlpha = 2.0 - (2.0 * dist * fadeRange);
  float offset = (2.0 * dist * fadeRange) - 1.0;
  new_position.y -= 2.0 * clamp(offset, 0, 1);
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
  int k0= 0; int k1= 1; int k2= 2;

  for (int i = 0; i < int(uLightCount);  i++) {
    if (uLightCastsShadowsArray[i] > 0.0) {
      lightSpacePosArray[k0] = uTexWorldViewProjMatrixArray[k0] * new_position;
      lightSpacePosArray[k1] = uTexWorldViewProjMatrixArray[k1] * new_position;
      lightSpacePosArray[k2] = uTexWorldViewProjMatrixArray[k2] * new_position;
    }

    k0 += 3; k1 += 3; k2 += 3;
  }
#endif

#ifdef HAS_REFLECTION
  const mat4 scalemat = mat4(0.5, 0.0, 0.0, 0.0,
                              0.0, 0.5, 0.0, 0.0,
                              0.0, 0.0, 0.5, 0.0,
                              0.5, 0.5, 0.5, 1.0);
  projectionCoord = scalemat * gl_Position;
#endif

#else //SHADOWCASTER
  gl_Position = uMVPMatrix * new_position;
#endif //SHADOWCASTER
}