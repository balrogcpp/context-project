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
attribute vec2 uv0;

#ifdef FADER_ENABLE
uniform float fadeRange;
#endif
#ifdef WAVER
uniform float uTime;
uniform vec4 uWaveDirection;
#endif

uniform vec3 uViewPos;
uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uTexWorldViewProjMatrixArray[3];

out vec4 lightSpacePosArray[3];
out vec3 vPosition;
out vec2 vUV;
out float depth;
uniform float NearClipDistance;
uniform float FarClipDistance;// !!! might be 0 for infinite view projection.

#ifdef FADER_ENABLE
out float fade;
#endif

void main()
{
  vec4 pos = uModelMatrix * position;
  vPosition = vec3(pos.xyz) / pos.w;
  vec4 mypos = position;

  vUV = uv0;

#ifdef WAVER
  if (vUV.xy == vec2(0.0)) {
    mypos.y += sin(uTime + position.z + position.y + position.x) * uWaveDirection.y;
    mypos.x += sin(uTime + position.z ) * uWaveDirection.x;
  }
#endif

  // Calculate the position of vertex in light space
  lightSpacePosArray[0] = uTexWorldViewProjMatrixArray[0] * mypos;
  lightSpacePosArray[1] = uTexWorldViewProjMatrixArray[1] * mypos;
  lightSpacePosArray[2] = uTexWorldViewProjMatrixArray[2] * mypos;

//   lightSpacePosArray[0] /= lightSpacePosArray[0].w;
//   lightSpacePosArray[1] /= lightSpacePosArray[1].w;
//   lightSpacePosArray[2] /= lightSpacePosArray[2].w;

  gl_Position = uMVPMatrix * mypos;
  depth = gl_Position.z;

#ifdef FADER_ENABLE
  fade = 1.0 - clamp(gl_Position.z / fadeRange, 0.0, 1.0);
#endif
}