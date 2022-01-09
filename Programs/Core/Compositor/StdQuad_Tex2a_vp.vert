// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef GL_ES
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
#endif
#include "header.vert"

layout(location = 0) uniform mat4 worldViewProj;

layout(location = 0) in vec4 vertex;
layout(location = 8) in vec2 uv0;

layout(location = 0) out vec2 oUv0;

void main()
{
    gl_Position = worldViewProj * vertex;
    oUv0 = uv0;
}
