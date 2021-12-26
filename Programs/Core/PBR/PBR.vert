// This source file is part of "glue project". Created by Andrew Vasiliev

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

uniform mat4 cWorldViewProjPrev;
#ifdef HAS_COLOURS
in vec3 colour;
#endif // HAS_COLOURS
uniform mat4 uModelMatrix;
#ifdef PAGED_GEOMETRY
uniform vec3 uCameraPosition;
#define HAS_UV
uniform float uFadeRange;
uniform float uWindRange;
#endif // PAGED_GEOMETRY
uniform float uTime;
out vec2 vUV0;
out float vDepth;
out float vAlpha;
#ifdef HAS_NORMALS
in vec3 normal;
#endif // HAS_NORMALS
#ifdef HAS_TANGENTS
in vec4 tangent;
#endif // HAS_TANGENTS
#ifdef SHADOWRECEIVER
uniform int uShadowTextureCount;
uniform mat4 uTexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
out vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif // SHADOWRECEIVER
out vec3 vPosition;
out vec4 vScreenPosition;
out vec4 vPrevScreenPosition;
#ifdef HAS_COLOURS
out vec3 vColor;
#endif // HAS_COLOURS
#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
out mat3 vTBN;
#else // !HAS_TANGENTS
out vec3 vNormal;
#endif // HAS_TANGENTS
#endif // HAS_NORMALS
#ifdef HAS_REFLECTION
out vec4 projectionCoord;
#endif // HAS_REFLECTION

#else // SHADOWCASTER
out vec2 vUV0;
#endif // !SHADOWCASTER

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
#include "noise.glsl"
#endif

void main()
{
#ifndef VERTEX_COMPRESSION
  vec4 new_position = vec4(position, 1.0);
#else
  vec4 new_position = posIndexToObjectSpace * vec4(vertex, uv0, 1.0);
#define uv0 _uv0
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
#endif // !SHADOWCASTER

  gl_Position = uMVPMatrix * new_position;

  vScreenPosition = gl_Position;
  vPrevScreenPosition = cWorldViewProjPrev * uModelMatrix * new_position;

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
