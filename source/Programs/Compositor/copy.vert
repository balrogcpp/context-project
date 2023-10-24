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

uniform highp mat4 WorldViewProj;

in highp vec4 vertex;
void main()
{
    gl_Position = mul(WorldViewProj, vertex);
}
