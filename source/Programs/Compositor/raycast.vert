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
IN(highp vec4 vertex, POSITION);
IN(highp vec4 normal, NORMAL);
OUT(mediump vec2 vUV0, TEXCOORD0);
OUT(highp vec3 vRay, TEXCOORD1);

MAIN_DECLARATION
{
    gl_Position = mul(WorldViewProj, vertex);

    highp vec2 inPos = sign(vertex.xy);
    vUV0 = (vec2(inPos.x, -inPos.y) + 1.0) * 0.5;
    vRay = normal.xyz;
}
