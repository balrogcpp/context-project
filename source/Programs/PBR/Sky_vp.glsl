// created by Andrey Vasiliev

#ifndef VERSION
#ifndef GL_ES
#version 330 core
#define VERSION 330
#else
#version 300 es
#define VERSION 300
#endif
#endif

#include "header.vert"

uniform mat4 worldViewProj;

in vec4 position;
in vec3 uv0;

out vec3 vPosition;
out vec3 TexCoords; // direction vector representing a 3D texture coordinate

//----------------------------------------------------------------------------------------------------------------------
void main() {
    TexCoords = uv0;
    vPosition = position.xyz;
    gl_Position = worldViewProj * position;
}
