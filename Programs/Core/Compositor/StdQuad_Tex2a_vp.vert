// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif
#include "header.vert"

layout(location = 0) uniform mat4 worldViewProj;

layout(location = 0) in vec4 vertex;
layout(location = 8) in vec2 uv0;

layout(location = 0) out vec2 vUV0;

void main()
{
    gl_Position = worldViewProj * vertex;
    vUV0 = uv0;
}
