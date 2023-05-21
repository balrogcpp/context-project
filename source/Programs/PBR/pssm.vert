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


IN(highp vec4 vertex, POSITION);
#ifdef SHADOWCASTER_ALPHA
IN(highp vec4 uv0, TEXCOORD0);
OUT(mediump vec2 vUV0, TEXCOORD0);
#endif
uniform highp mat4 WorldViewProjMatrix;


MAIN_DECLARATION
{
#ifdef SHADOWCASTER_ALPHA
    vUV0.xy = uv0.xy;
#endif

   gl_Position = mul(WorldViewProjMatrix, vertex);
}
