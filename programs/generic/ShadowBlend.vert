#ifndef GL_ES
#version 120
#else
#version 100
#endif

uniform mat4 worldViewProj;
in vec4 vertex;

void main() {
    gl_Position = worldViewProj*vertex;
}
