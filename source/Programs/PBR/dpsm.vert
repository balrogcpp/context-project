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


IN(highp vec4 vertex, POSITION)
#ifdef SHADOWCASTER_ALPHA
IN(highp vec4 uv0, TEXCOORD0)
OUT(mediump vec2 vUV0, TEXCOORD0)
#endif
uniform highp mat4 WorldViewMatrix;


MAIN_DECLARATION
{
#ifdef SHADOWCASTER_ALPHA
    vUV0.xy = uv0.xy;
#endif

    gl_Position = mul(WorldViewMatrix, vertex);
    highp float L = length(gl_Position.xyz);
    gl_Position = gl_Position / L;
    gl_Position.z = gl_Position.z + 1.0;
    gl_Position.x = gl_Position.x / gl_Position.z;
    gl_Position.y = gl_Position.y / gl_Position.z;
    gl_Position.z = L;
    gl_Position.w = 1.0;
}
