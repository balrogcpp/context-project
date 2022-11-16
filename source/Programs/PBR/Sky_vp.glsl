// created by Andrey Vasiliev

#ifndef __VERSION__
#ifndef GL_ES
#version 330 core
#define __VERSION__ 330
#else
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.vert"

uniform mat4 worldViewProj;

in vec4 position;
in vec4 uv0;

out vec3 vPosition;
out vec3 vTexCoords; // direction vector representing a 3D texture coordinate

//----------------------------------------------------------------------------------------------------------------------
void main() {
    vTexCoords = uv0.xyz;
    vPosition = position.xyz;
    gl_Position = worldViewProj * position;
}
