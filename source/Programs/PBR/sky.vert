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
in highp vec4 uv0;
out highp vec3 vPosition;
out highp vec3 vUV0;
void main()
{
    vUV0 = uv0.xyz;
    vPosition = vertex.xyz;
    gl_Position = mul(WorldViewProjMatrix, vertex);
}
