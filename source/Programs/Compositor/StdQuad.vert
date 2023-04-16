// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"


in highp vec4 vertex;
out mediump vec2 vUV0;
uniform highp mat4 WorldViewProj;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    gl_Position = mul(WorldViewProj, vertex);

    vec2 inPos = sign(vertex.xy);
    vUV0 = (vec2(inPos.x, -inPos.y) + 1.0) * 0.5;
}
