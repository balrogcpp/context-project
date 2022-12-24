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
uniform highp mat4 uWorldViewProjPrev;
uniform lowp float uStaticObj;
uniform lowp float uMovableObj;
#ifdef PAGED_GEOMETRY
uniform highp float Time;
#endif
#ifdef GRASS
uniform highp vec3 CameraPosition;
//uniform float uFadeRange;
uniform float uWindRange;
#endif //  GRASS
#ifdef SHADOWRECEIVER
uniform int ShadowTextureCount;
uniform mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
#endif // SHADOWRECEIVER


// in block
in highp vec4 position;
#ifdef HAS_NORMALS
in vec4 normal;
#endif
#ifdef HAS_TANGENTS
in vec4 tangent;
#endif
#ifdef HAS_VERTEXCOLOR
in vec4 colour;
#endif
#ifdef HAS_UV
in vec2 uv0;
#ifdef TREES
in vec4 uv1;
in vec4 uv2;
#endif
#endif //  HAS_UV


// out block
out vec2 vUV0;
out float vDepth;
out highp vec3 vPosition;
out vec3 vColor;
out vec4 vScreenPosition;
out vec4 vPrevScreenPosition;
out mat3 vTBN;
#ifdef SHADOWRECEIVER
out vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif


#ifdef GRASS
//----------------------------------------------------------------------------------------------------------------------
vec4 WaveGrass(const vec4 position, const float time, const float frequency, const vec4 direction)
{
  float n = noise(position.xz * time) * 2.0 - 2.0;
  return n * direction;
}
#endif


#ifdef TREES
//----------------------------------------------------------------------------------------------------------------------
vec4 WaveTree(const vec4 v, const vec4 params, const vec4 position, const float time)
{
  float radiusCoeff = params.x;
  float radiusCoeff2 = radiusCoeff * radiusCoeff;
  float heightCoeff = params.y;
  float heightCoeff2 = heightCoeff * heightCoeff;
  float factorX = params.z;
  float factorY = params.w;

  vec4 ret = vec4(0.0);
  ret.y = sin(time + position.z + v.y + v.x) * radiusCoeff2 * factorY;
  ret.x = sin(time + position.z ) * heightCoeff2 * factorX;

  return ret;
}
#endif


//----------------------------------------------------------------------------------------------------------------------
void main()
{
#ifdef HAS_UV
  vec4 new_position = position;
  vUV0.xy = uv0.xy;
#else
  vUV0.xy = vec2(0.0);
#endif


#ifdef HAS_VERTEXCOLOR
  vColor = colour.rgb;
#else
  vColor = vec3(1.0);
#endif

  vec4 model_position = ModelMatrix * new_position;
  vPosition = model_position.xyz / model_position.w;

#ifdef GRASS
  if (uv0.y < 0.5 && distance(CameraPosition.xyz, vPosition.xyz) < uWindRange) {
    new_position += WaveGrass(new_position, 0.2 * Time, 1.0, vec4(0.5, 0.1, 0.25, 0.0));
  }
#endif
#ifdef TREES
    new_position += WaveTree(new_position, uv1, uv2, Time);
#endif

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
  vec3 n = normalize(vec3(ModelMatrix * vec4(normal.xyz, 0.0)));
  vec3 t = normalize(vec3(ModelMatrix * vec4(tangent.xyz, 0.0)));
  vec3 b = cross(n, t) * tangent.w;
  vTBN = mat3(t, b, n);
#else // NO HAS_TANGENTS
  vec3 n = normalize(vec3(ModelMatrix * vec4(normal.xyz, 0.0)));
  vec3 b = normalize(cross(n, vec3(1.0, 0.0, 0.0)));
  vec3 t = normalize(cross(n ,b));
  //vec3 t = vec3(1.0, 0.0, 0.0);
  //vec3 b = cross(n, t) * tangent.w;
  vTBN = mat3(t, b, n);
#endif
#else // !HAS_NORMALS
  vTBN = mat3(vec3(1.0, 0.0, 0.0),
              vec3(0.0, 0.0, 0.0),
              vec3(0.0, 1.0, 0.0));
#endif // HAS_NORMALS

  gl_Position = MVPMatrix * new_position;

  vScreenPosition = gl_Position;
  vPrevScreenPosition = uStaticObj * uWorldViewProjPrev * ModelMatrix * new_position + uMovableObj * uWorldViewProjPrev * new_position;

  vDepth = gl_Position.z;

#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
  // Calculate the position of vertex in light space
  for (int i = 0; i < MAX_SHADOW_TEXTURES; ++i) {
    if (ShadowTextureCount <= i) break;
    
    vLightSpacePosArray[i] = TexWorldViewProjMatrixArray[i] * new_position;
  }
#endif
#endif
}
