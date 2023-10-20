// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"

uniform highp mat4 WorldViewProj;

in highp vec4 vertex;
out vec2 vUV0;
void main()
{
    gl_Position = mul(WorldViewProj, vertex);

    vec2 inPos = sign(vertex.xy);
    vUV0 = (vec2(inPos.x, -inPos.y) + 1.0) * 0.5;
}
