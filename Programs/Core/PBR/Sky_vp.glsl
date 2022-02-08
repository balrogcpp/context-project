// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef VERSION
#ifndef GL_ES
#version 330 core
#define VERSION 330
#else
#version 100
#define VERSION 100
#endif
#endif

#include "header.vert"

uniform mat4 worldViewProj;

in vec4 position;
in vec3 uv0;

#ifdef NO_MRT
out float vDepth;
#endif
out vec3 TexCoords; // direction vector representing a 3D texture coordinate

void main() {
    TexCoords = position.xyz;
    gl_Position = worldViewProj * position;
#ifdef NO_MRT
    vDepth = gl_Position.z;
#endif
}
