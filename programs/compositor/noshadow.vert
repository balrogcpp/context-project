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

out vec2 vUV;

uniform vec3 uViewPos;
uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;

void main () {
  vUV = uv0;
  gl_Position = uMVPMatrix * position;
}