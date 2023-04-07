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


#include "header.vert"


in highp vec4 position;
in mediump vec4 uv0;
out highp vec3 vPosition;
out mediump vec3 vUV0; // direction vector representing a 3D texture coordinate
uniform highp mat4 WorldViewProj;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    vUV0 = uv0.xyz;
    vPosition = position.xyz;
    gl_Position = WorldViewProj * position;
}
