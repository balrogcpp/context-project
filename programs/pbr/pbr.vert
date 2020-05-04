// The MIT License
// Copyright (c) 2016-2017 Mohamad Moneimne and Contributors

#ifndef GL_ES
#define VERSION 120
#version VERSION
#if VERSION != 120
#define attribute in
#else
#define in varying
#define out varying
#endif
#else
#version 100
#define in varying
#define out varying
#endif

attribute vec4 position;
#ifdef HAS_NORMALS
attribute vec4 normal;
#endif
#ifdef HAS_TANGENTS
attribute vec4 tangent;
#endif
#ifdef HAS_UV
attribute vec2 uv0;
#endif

uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;
uniform float uLightCount;

#define MAX_LIGHTS 5

#ifdef DEPTH_SHADOWRECEIVER
uniform float uLightCastsShadowsArray[MAX_LIGHTS];
uniform mat4 uTexWorldViewProjMatrixArray[3];
out vec4 lightSpacePosArray[MAX_LIGHTS * 3];
#endif

#ifdef FADER_ENABLE
uniform float fadeRange;
#endif
#ifdef WAVER
uniform float uTime;
uniform vec4 uWaveDirection;
#endif

out vec3 vPosition;
out vec4 vUV;

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
out mat3 vTBN;
#else
out vec3 vNormal;
#endif
#endif

void main()
{
  vec4 pos = uModelMatrix * position;
  vPosition = vec3(pos.xyz) / pos.w;
  vec4 mypos = position;

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

#ifdef WAVER
    if (uv0 == vec2(0.0)) {
      mypos.y += sin(uTime*2.0 + position.z + position.y + position.x) * 0.05;
      mypos.x += sin(uTime*2.0 + position.z ) * 0.05;
    }
#endif

  for (int i = 0; i < int(uLightCount);  i += 3) {
#ifdef DEPTH_SHADOWRECEIVER
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

#ifdef FADER_ENABLE
  vUV.w = 1.0 - clamp(gl_Position.z / fadeRange, 0.0, 1.0);
#endif
}