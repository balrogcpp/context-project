// created by Andrey Vasiliev

#ifndef __VERSION__
#ifndef GL_ES
#version 330 core
#define __VERSION__ 330
#else
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.vert"

// in block
in vec4 position;
#ifdef HAS_NORMALS
in vec4 normal;
#endif
#ifdef HAS_TANGENTS
in vec4 tangent;
#endif
#ifdef HAS_COLORS
in vec4 color;
#endif
#ifdef HAS_UV
in vec4 uv0;
#ifdef TREES
in vec4 uv1;
in vec4 uv2;
#endif
#endif


// uniform block
uniform mat4 MVPMatrix;
uniform mat4 ModelMatrix;
#ifndef NO_MRT
uniform mat4 uWorldViewProjPrev;
#endif
#ifdef PAGED_GEOMETRY
uniform vec3 CameraPosition;
uniform float Time;
uniform float uFadeRange;
#ifdef GRASS
uniform float uWindRange;
#endif
#endif // PAGED_GEOMETRY
#ifdef SHADOWRECEIVER
uniform int ShadowTextureCount;
uniform mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
#endif // SHADOWRECEIVER


// out block
out vec2 vUV0;
out float vDepth;
out vec3 vPosition;
#ifndef NO_MRT
out vec4 vScreenPosition;
out vec4 vPrevScreenPosition;
#endif
#ifdef HAS_COLORS
out vec3 vColor;
#endif
#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
out mat3 vTBN;
#else
out vec3 vNormal;
#endif
#endif // HAS_NORMALS
#ifdef SHADOWRECEIVER
out vec4 LightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif
#ifdef HAS_REFLECTION
out vec4 projectionCoord;
#endif


#ifdef GRASS
#include "noise.glsl"
#endif

#ifdef HAS_REFLECTION
//----------------------------------------------------------------------------------------------------------------------
vec4 GetProjectionCoord(const vec4 position) {
  return mat4(
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 0.5, 0.0,
  0.5, 0.5, 0.5, 1.0
  ) * position;
}
#endif

#ifdef GRASS
//----------------------------------------------------------------------------------------------------------------------
vec4 ApplyWaveAnimation(const vec4 position, const float time, const float frequency, const vec4 direction)
{
  float n = fbm(position.xy * time) * 2.0 - 2.0;
  return position + n * direction;
}
#endif

#ifdef TREES
//----------------------------------------------------------------------------------------------------------------------
vec4 WaveTree(const vec4 v)
{
  vec4 params = uv1;
  vec4 originPos = uv2;

  float radiusCoeff = params.x;
  float heightCoeff = params.y;
  float factorX = params.z;
  float factorY = params.w;

  vec4 result = v;
  result.y += sin(Time + originPos.z + result.y + result.x) * radiusCoeff * radiusCoeff * factorY;
  result.x += sin(Time + originPos.z ) * heightCoeff * heightCoeff * factorX;

  return result;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec4 new_position = position;

#ifdef HAS_UV
  vUV0.xy = uv0.xy;
#else
  vUV0.xy = vec2(0.0);
#endif

#ifdef HAS_COLORS
  vColor = color.rgb;
#endif

  vec4 model_position = ModelMatrix * new_position;
  vPosition = model_position.xyz / model_position.w;

#ifdef PAGED_GEOMETRY
#ifdef GRASS
  if (uv0.y < 0.9 && distance(CameraPosition.xyz, vPosition.xyz) < uWindRange)
    new_position = ApplyWaveAnimation(new_position, 0.2 * Time, 1.0, vec4(0.25, 0.1, 0.25, 0.0));
#endif
#ifdef TREES
    new_position = WaveTree(new_position);
#endif
#endif // PAGED_GEOMETRY

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
  vec3 n = normalize(vec3(ModelMatrix * vec4(normal.xyz, 0.0)));
  vec3 t = normalize(vec3(ModelMatrix * vec4(tangent.xyz, 0.0)));
  vec3 b = cross(n, t) * tangent.w;
  vTBN = mat3(t, b, n);
#else // HAS_TANGENTS != 1
  vNormal = normalize(vec3(ModelMatrix * vec4(normal.xyz, 0.0)));
#endif
#else // !HAS_NORMALS

#endif // HAS_NORMALS

  gl_Position = MVPMatrix * new_position;

#ifndef NO_MRT
  vScreenPosition = gl_Position;
  vPrevScreenPosition = uWorldViewProjPrev * ModelMatrix * new_position;
#endif

  vDepth = gl_Position.z;

#ifdef SHADOWRECEIVER
  // Calculate the position of vertex in light space
  for (int i = 0; i < ShadowTextureCount; i++) {
    LightSpacePosArray[i] = TexWorldViewProjMatrixArray[i] * new_position;
  }
#endif

#ifdef HAS_REFLECTION
  projectionCoord = GetProjectionCoord(gl_Position);
#endif
}
