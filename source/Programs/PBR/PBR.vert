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
#include "math.glsl"
#ifdef PAGED_GEOMETRY
#include "noise.glsl"
#endif


#ifdef PAGED_GEOMETRY
//----------------------------------------------------------------------------------------------------------------------
highp vec4 WaveGrass(const highp vec4 position, const highp float time, const float frequency, const vec4 direction)
{
  highp float n = NoiseHp(position.xz * time) * 2.0 - 2.0;
  return n * direction;
}

//----------------------------------------------------------------------------------------------------------------------
highp vec4 WaveTree(const highp vec4 position, const highp float time, const vec4 params1, const vec4 params2)
{
  highp float radiusCoeff = params1.x;
  highp float radiusCoeff2 = radiusCoeff * radiusCoeff;
  highp float heightCoeff = params1.y;
  highp float heightCoeff2 = heightCoeff * heightCoeff;
  highp float factorX = params1.z;
  highp float factorY = params1.w;

  return vec4(
    sin(time + params2.z ) * heightCoeff2 * factorX,
    sin(time + params2.z + position.y + position.x) * radiusCoeff2 * factorY,
    0.0, 0.0
  );
}
#endif


// uniform block
uniform highp mat4 MVPMatrix;
uniform highp mat4 ModelMatrix;
uniform highp mat4 uWorldViewProjPrev;
uniform lowp float uStaticObj;
uniform lowp float uMovableObj;
#ifdef PAGED_GEOMETRY
uniform highp vec4 Time;
uniform highp vec3 CameraPosition;
uniform float uFadeRange;
#endif
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
in vec4 uv0;
in vec4 uv1;
in vec4 uv2;
#endif //  HAS_UV


// out block
out vec2 vUV0;
out float vDepth;
out highp vec3 vPosition;
out vec4 vColor;
out vec4 vScreenPosition;
out vec4 vPrevScreenPosition;
out mat3 vTBN;
#ifdef SHADOWRECEIVER
out vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif


//----------------------------------------------------------------------------------------------------------------------
void main()
{
#ifdef HAS_UV
  vUV0.xy = uv0.xy;
#endif

#ifdef HAS_VERTEXCOLOR
  vColor = max3(colour.rgb) > 0.0 ? colour.rgba : vec4(1.0);
#else
  vColor = vec4(1.0);
#endif
  vec4 new_position = position;
  vec4 model_position = ModelMatrix * new_position;
  vPosition = model_position.xyz / model_position.w;

#ifdef PAGED_GEOMETRY
   new_position +=  uv2.x == 0.0 ? bigger(0.5, uv0.y) * WaveGrass(new_position, Time.x, 1.0, vec4(0.5, 0.1, 0.25, 0.0)) : WaveTree(new_position, Time.x, uv1, uv2);
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
  vec3 t = normalize(cross(n, b));
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
