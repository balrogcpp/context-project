// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif

#include "header.glsl"


attribute highp vec4 vertex;
#ifdef SHADOWCASTER_ALPHA
attribute highp vec2 uv0;
varying mediump vec2 vUV0;
#endif
uniform highp mat4 WorldViewProjMatrix;


void main()
{
#ifdef SHADOWCASTER_ALPHA
    vUV0.xy = uv0.xy;
#endif

   gl_Position = mul(WorldViewProjMatrix, vertex);
}
