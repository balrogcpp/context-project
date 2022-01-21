// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif
#include "header.vert"

uniform mat4 worldViewProj;
in vec4 vertex;
in vec2 uv0;
out vec2 oUv0;
out vec2 oUv1;
out vec4 pos;

void main()
{
	// Use standardise transform, so work accord with render system specific (RS depth, requires texture flipping, etc)
    gl_Position = worldViewProj * vertex;

    oUv0 = uv0;
    oUv1 = -vertex.xy;
    pos = gl_Position;
}
