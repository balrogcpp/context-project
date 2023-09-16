// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"

uniform highp mat4 WorldViewProjMatrix;

in highp vec4 vertex;
#ifdef SHADOWCASTER_ALPHA
in highp vec4 uv0;
out mediump vec2 vUV0;
#endif
void main()
{
#ifdef SHADOWCASTER_ALPHA
    vUV0.xy = uv0.xy;
#endif

   gl_Position = mul(WorldViewProjMatrix, vertex);
}
