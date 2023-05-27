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

//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(highp vec4 vertex, POSITION)
IN(highp vec4 uv0, TEXCOORD0)
OUT(highp vec3 vPosition, TEXCOORD0)
OUT(highp vec3 vUV0, TEXCOORD1)

MAIN_DECLARATION
{
    vUV0 = uv0.xyz;
    vPosition = vertex.xyz;
    gl_Position = mul(WorldViewProj, vertex);
}
