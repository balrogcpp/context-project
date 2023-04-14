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


in highp vec4 position;
out highp vec3 vModelPosition;
out highp vec3 vViewPosition;
out highp vec4 vScreenPosition;
out highp vec4 vPrevScreenPosition;

uniform highp mat4 ModelMatrix;
uniform highp mat4 WorldViewMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;


void main()
{
    highp vec4 model = ModelMatrix * position;
    vModelPosition = model.xyz / model.w;
    highp vec4 view = WorldViewMatrix * position;
    vViewPosition = view.xyz / view.w;

    gl_Position = WorldViewProjMatrix * position;

    vScreenPosition = gl_Position;
    vPrevScreenPosition = WorldViewProjPrev * position;
}
