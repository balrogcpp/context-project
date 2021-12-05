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

in vec4 position;
in vec3 uv0;
out float vDepth;
out vec3 TexCoords; // direction vector representing a 3D texture coordinate

void main() {
    TexCoords = uv0.xyz;
    gl_Position = worldViewProj * position;
    vDepth = gl_Position.z;
}
