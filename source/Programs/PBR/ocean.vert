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
#include "math.glsl"

in vec4 vertex;
in vec4 uv0;

uniform mat4 ModelMatrix;
uniform mat4 WorldMatrix;
uniform vec3 eyePosition;
uniform vec3 cameraPos;

out vec3 viewPos;
out vec3 worldPos;
out vec4 projectionCoord;


void main()
{
    projectionCoord = vec4(WorldMatrix * vertex);

    worldPos = vec3(uv0);

    vec3 pos = vec3(vertex);
    viewPos = pos - eyePosition;

    gl_Position = WorldMatrix * vertex;

    if (cameraPos.y < 0.0)
    {
        gl_Position = vec4(vertex.xzy, 1.0);
    }
}
