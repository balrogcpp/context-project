// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.vert"

in vec4 vertex;
uniform mat4 worldViewProj;

out vec2 vUV0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    gl_Position = worldViewProj * vertex;

    vec2 inPos = sign(vertex.xy);
    vUV0 = (vec2(inPos.x, -inPos.y) + 1.0) * 0.5;
}
