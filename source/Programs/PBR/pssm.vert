// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif

#include "header.glsl"


in highp vec4 vertex;
#ifdef SHADOWCASTER_ALPHA
in highp vec2 uv0;
out mediump vec2 vUV0;
#endif
uniform highp mat4 WorldViewProjMatrix;


void main()
{
#ifdef SHADOWCASTER_ALPHA
    vUV0.xy = uv0.xy;
#endif

   gl_Position = mul(WorldViewProjMatrix, vertex);
}
