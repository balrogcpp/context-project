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
in vec4 normal;
out vec2 vUV0;
out vec3 vRay;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    gl_Position = vertex;

    vec2 inPos = sign(vertex.xy);
    vUV0 = (vec2(inPos.x, inPos.y) + 1.0) * 0.5;
    vRay = normal.xyz;
}
