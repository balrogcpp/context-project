// This source file is part of "glue project". Created by Andrew Vasiliev

#ifndef GL_ES
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
#endif
#include "header.vert"

uniform mat4 worldViewProj;
in vec4 vertex;
in vec2 uv0;
out vec2 oUv0;
out vec2 oUv1;

void main()
{
	// Use standardise transform, so work accord with render system specific (RS depth, requires texture flipping, etc)
    gl_Position = worldViewProj * vertex;

    // Convert to image-space
    oUv0 = uv0;
    oUv1 = oUv0;
}
