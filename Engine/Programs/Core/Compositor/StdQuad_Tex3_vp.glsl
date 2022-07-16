// This source file is part of Glue Engine. Created by Andrey Vasiliev

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
out vec2 vUV0;
out vec2 vUV1;
out vec2 vUV2;
out vec4 pos;

void main()
{
	// Use standardise transform, so work accord with render system specific (RS depth, requires texture flipping, etc)
    gl_Position = worldViewProj * vertex;

    // Convert to image-space
    vUV0 = uv0;
    vUV1 = vUV0;
    vUV2 = vUV0;
    pos = gl_Position;
}
