// This source file is part of "glue project". Created by Andrew Vasiliev

#ifndef GL_ES
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
#endif
#include "header.vert"

in vec4 vertex;
uniform mat4 worldViewProj;

out vec2 oUv0;

void main()
{
    gl_Position = worldViewProj * vertex;

    vec2 inPos = sign(vertex.xy);
    oUv0 = (vec2(inPos.x, -inPos.y) + 1.0) * 0.5;
}
