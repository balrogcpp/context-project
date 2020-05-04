#ifndef GL_ES
#define VERSION 120
#version VERSION
#if VERSION != 120
#define attribute in
#else
#define in varying
#define out varying
#endif
#else
#version 100
#define in varying
#define out varying
#endif

uniform mat4 worldViewProj;

attribute vec4 position;
attribute vec4 uv0;

out vec3 TexCoords; // direction vector representing a 3D texture coordinate

void main() {
    TexCoords = uv0.xyz;
    gl_Position = worldViewProj * position;
}
