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
#ifdef GRASS
#include "noise.glsl"
#endif


#ifndef MAX_SHADOW_TEXTURES
#define MAX_SHADOW_TEXTURES 0
#endif


// uniform block
uniform highp mat4 MVPMatrix;
uniform highp mat4 ModelMatrix;
#ifndef NO_MRT
uniform highp mat4 uWorldViewProjPrev;
#endif
#ifdef PAGED_GEOMETRY
uniform highp vec3 CameraPosition;
uniform highp float Time;
uniform float uFadeRange;
#ifdef GRASS
uniform float uWindRange;
#endif
#endif // PAGED_GEOMETRY
#ifdef SHADOWRECEIVER
uniform int ShadowTextureCount;
uniform mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
#endif // SHADOWRECEIVER


// in block
#ifndef VERTEX_COMPRESSION
in highp vec3 position;
#else
in vec2 vertex;
in float uv0;
uniform mat4 posIndexToObjectSpace;
uniform float baseUVScale;
#endif
#ifdef HAS_NORMALS
in vec4 normal;
#endif
#ifdef HAS_TANGENTS
in vec4 tangent;
#endif
#ifdef HAS_COLORS
in vec4 colour;
#endif
#ifndef VERTEX_COMPRESSION
#ifdef HAS_UV
in vec2 uv0;
#ifdef TREES
in vec4 uv1;
in vec4 uv2;
#endif
#endif //  HAS_UV
#endif //  !VERTEX_COMPRESSION


// out block
out vec2 vUV0;
out float vDepth;
out highp vec3 vPosition;
out vec3 vColor;
#ifndef NO_MRT
out vec4 vScreenPosition;
out vec4 vPrevScreenPosition;
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
out mat3 vTBN;
#endif


#ifdef GRASS
//----------------------------------------------------------------------------------------------------------------------
vec4 ApplyWaveAnimation(const vec2 position, const float time, const float frequency, const vec4 direction)
{
  float n = fbm(position.xy * time) * 2.0 - 2.0;
  return n * direction;
}
#endif


#ifdef TREES
//----------------------------------------------------------------------------------------------------------------------
vec4 WaveTree(const vec4 v)
{
  vec4 params = uv1;
  vec4 originPos = uv2;
  float radiusCoeff = params.x;
  float radiusCoeff2 = radiusCoeff * radiusCoeff;
  float heightCoeff = params.y;
  float heightCoeff2 = heightCoeff * heightCoeff;
  float factorX = params.z;
  float factorY = params.w;

  vec4 ret = vec4(0.0);
  ret.y = sin(Time + originPos.z + v.y + v.x) * radiusCoeff2 * factorY;
  ret.x = sin(Time + originPos.z ) * heightCoeff2 * factorX;

  return ret;
}
#endif


//----------------------------------------------------------------------------------------------------------------------
void main()
{
#ifdef HAS_UV
#ifndef VERTEX_COMPRESSION
  vec4 new_position = vec4(position, 1.0);
  vec2 new_uv = uv0.xy;
#else
  vec4 new_position = posIndexToObjectSpace * vec4(vertex, uv0, 1.0);
  vec2 new_uv = vec2(vertex.x * baseUVScale, 1.0 - (vertex.y * baseUVScale));
#endif

  vUV0.xy = new_uv.xy;
#else
  vUV0.xy = vec2(0.0);
#endif


#ifdef HAS_COLORS
  vColor = colour.rgb;
#else
  vColor = vec3(1.0);
#endif

  vec4 model_position = ModelMatrix * new_position;
  vPosition = model_position.xyz / model_position.w;

#ifdef PAGED_GEOMETRY
#ifdef GRASS
  vTBN = mat3(vec3(1.0, 0.0, 0.0),
              vec3(0.0, 0.0, 0.0),
              vec3(0.0, 1.0, 0.0));

  if (uv0.y < 0.9 && distance(CameraPosition.xyz, vPosition.xyz) < uWindRange) {
    new_position += ApplyWaveAnimation(new_position.xy, 0.2 * Time, 1.0, vec4(0.25, 0.1, 0.25, 0.0));
  }
#endif
#ifdef TREES
    new_position += WaveTree(new_position);
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
#ifndef PAGED_GEOMETRY
  vPrevScreenPosition = uWorldViewProjPrev * new_position;
#else
  vPrevScreenPosition = uWorldViewProjPrev * ModelMatrix * new_position;
#endif
#endif

  vDepth = gl_Position.z;

#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
  // Calculate the position of vertex in light space
  for (int i = 0; i < MAX_SHADOW_TEXTURES; ++i) {
    if (ShadowTextureCount <= i) break;
    LightSpacePosArray[i] = TexWorldViewProjMatrixArray[i] * new_position;
  }
#endif
#endif

#ifdef HAS_REFLECTION
  projectionCoord = mat4(0.5, 0.0, 0.0, 0.0,
                         0.0, 0.5, 0.0, 0.0,
                         0.0, 0.0, 0.5, 0.0,
                         0.5, 0.5, 0.5, 1.0
                         ) * gl_Position;
#endif
}