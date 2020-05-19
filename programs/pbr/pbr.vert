// The MIT License
// Copyright (c) 2016-2017 Mohamad Moneimne and Contributors

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
uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;

#ifdef SHADOWCASTER_ALPHA
#ifdef HAS_UV
attribute vec2 uv0;
#endif

out vec4 vUV;
#endif

#ifndef SHADOWCASTER
#ifdef HAS_UV
attribute vec2 uv0;
#endif

out vec4 vUV;

#ifdef HAS_NORMALS
attribute vec4 normal;
#endif
#ifdef HAS_TANGENTS
attribute vec4 tangent;
#endif

uniform float uLightCount;

#define MAX_LIGHTS 5

#ifdef SHADOWRECEIVER
uniform float uLightCastsShadowsArray[MAX_LIGHTS];
uniform mat4 uTexWorldViewProjMatrixArray[3];
out vec4 lightSpacePosArray[MAX_LIGHTS * 3];
#endif

#ifdef FADE
uniform float fadeRange;
#endif
uniform float uTime;

#ifdef TERRAIN
uniform sampler2D HeighMap;
uniform vec4 TerrainBox; //-x+x-z+z
uniform vec4 TerrainBox2; //-y+y
#endif

out vec3 vPosition;

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
out mat3 vTBN;
#else
out vec3 vNormal;
#endif
#endif

#endif //SHADOWCASTER

void main()
{
  vec4 mypos = position;

#ifdef TERRAIN
  vec2 uv;
  uv.x = (mypos.x - TerrainBox.x) / (TerrainBox.y - TerrainBox.x);
  uv.y = (mypos.z - TerrainBox.z) / (TerrainBox.w - TerrainBox.z);
  float heigh = texture2D(HeighMap, uv).r * TerrainBox2.y - TerrainBox2.x;
  mypos.y += heigh;
#endif

#ifdef FOREST
  float radiusCoeff = 0.25;
  float heightCoeff = 0.25;
  float factorX = 1;
  float factorY = 1;
  mypos.y += sin(uTime + mypos.z + mypos.y + mypos.x) * radiusCoeff * radiusCoeff * factorY;
  mypos.x += sin(uTime + mypos.z ) * heightCoeff * heightCoeff * factorX;
#endif

#ifndef SHADOWCASTER
  vec4 pos = uModelMatrix * position;

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
  vec3 normalW = normalize(vec3(uModelMatrix * vec4(normal.xyz, 0.0)));
  vec3 tangentW = normalize(vec3(uModelMatrix * vec4(tangent.xyz, 0.0)));
  vec3 bitangentW = cross(normalW, tangentW) * tangent.w;
  vTBN = mat3(tangentW, bitangentW, normalW);
#else // HAS_TANGENTS != 1
  vNormal = normalize(vec3(uModelMatrix * vec4(normal.xyz, 0.0)));
#endif
#endif

#ifdef HAS_UV
  vUV.xy = uv0;
#else
  vUV.xy = vec2(0.0);
#endif

  for (int i = 0; i < int(uLightCount);  i += 3) {
#ifdef SHADOWRECEIVER
  // Calculate the position of vertex in light space
  if (uLightCastsShadowsArray[i] == 1.0) {
    lightSpacePosArray[i] = uTexWorldViewProjMatrixArray[i] * mypos;
    lightSpacePosArray[i + 1] = uTexWorldViewProjMatrixArray[i + 1] * mypos;
    lightSpacePosArray[i + 2] = uTexWorldViewProjMatrixArray[i + 2] * mypos;

    lightSpacePosArray[i] /= lightSpacePosArray[i].w;
    lightSpacePosArray[i + 1] /= lightSpacePosArray[i + 1].w;
    lightSpacePosArray[i + 2] /= lightSpacePosArray[i + 2].w;
  }
#endif
  }

  gl_Position = uMVPMatrix * mypos;
  vUV.z = gl_Position.z;
  vPosition = vec3(pos.xyz) / pos.w;

#else //SHADOWCASTER

#ifdef SHADOWCASTER_ALPHA
#ifdef HAS_UV
  vUV.xy = uv0;
#else
  vUV.xy = vec2(0.0);
#endif
#endif

  gl_Position = uMVPMatrix * mypos;
#endif //SHADOWCASTER

#ifdef FADE
  vUV.w = 1.0 - clamp(gl_Position.z * fadeRange, 0.0, 1.0);
#endif
}