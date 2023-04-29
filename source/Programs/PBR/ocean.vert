// created by Andrey Vasiliev

#ifndef __VERSION__
#ifndef GL_ES
#version 330 core
#define __VERSION__ 330
#else
#version 300 es
#define __VERSION__ 300
#endif
#endif


#include "header.glsl"
#include "math.glsl"


in highp vec4 vertex;

out highp vec3 vWorldPosition;
out mediump mat3 vTBN;
out mediump vec4 vScreenPosition;
out mediump vec4 vPrevScreenPosition;

uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform highp vec4 CameraPosition;
uniform mediump vec4 Time;

void main()
{
    highp vec4 position = vertex;

    highp vec4 world = mul(WorldMatrix, position);
    vWorldPosition = world.xyz / world.w;
    gl_Position = mul(WorldViewProjMatrix, position);

    vScreenPosition = gl_Position;
    vPrevScreenPosition = mul(WorldViewProjPrev, position);
}
