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
#include "ocean.glsl"


in highp vec4 vertex;
out highp vec3 vModelPosition;
out highp vec3 vViewPosition;
out mediump vec4 vScreenPosition;
out mediump vec4 vPrevScreenPosition;

uniform highp mat4 ModelMatrix;
uniform highp mat4 WorldViewMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;


void main()
{
    highp vec4 model = mul(ModelMatrix, vertex);
    vModelPosition = model.xyz / model.w;
    highp vec4 view = mul(WorldViewMatrix, vertex);
    vViewPosition = view.xyz / view.w;

    gl_Position = mul(WorldViewProjMatrix, vertex);

    vScreenPosition = gl_Position;
    vPrevScreenPosition = mul(WorldViewProjPrev, vertex);
}
