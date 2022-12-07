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


uniform highp mat4 worldViewProj;


in highp vec4 position;
in vec4 uv0;


out highp vec3 vPosition;
//out vec3 vUV0; // direction vector representing a 3D texture coordinate


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    //vUV0 = uv0.xyz;
    vPosition = position.xyz;
    gl_Position = worldViewProj * position;
}
