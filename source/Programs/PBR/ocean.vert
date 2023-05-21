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
uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform mediump vec4 Time;


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(highp vec4 vertex, POSITION);
OUT(highp vec3 vWorldPosition, TEXCOORD0);
OUT(mediump vec4 vScreenPosition, TEXCOORD1);
OUT(mediump vec4 vPrevScreenPosition, TEXCOORD2);

MAIN_DECLARATION
{
    highp vec4 position = vertex;

    highp vec4 world = mul(WorldMatrix, position);
    vWorldPosition = world.xyz / world.w;
    gl_Position = mul(WorldViewProjMatrix, position);

    vScreenPosition = gl_Position;
    vPrevScreenPosition = mul(WorldViewProjPrev, position);
}
