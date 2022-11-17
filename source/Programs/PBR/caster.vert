// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif

#include "header.vert"


in vec4 position;

#ifdef SHADOWCASTER_ALPHA
in vec2 uv0;
out vec2 vUV0;
#endif

uniform mat4 MVPMatrix;


void main()
{
#ifdef SHADOWCASTER_ALPHA
  vUV0.xy = uv0.xy;
#endif

  gl_Position = MVPMatrix * position;
}