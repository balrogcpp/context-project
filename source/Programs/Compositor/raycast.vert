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

in highp vec3 vertex;
in highp vec3 normal;
in vec2 uv0;

out highp vec2 vUV0;
out highp vec3 vRay;
void main()
{
    gl_Position = mul(WorldViewProj, vec4(vertex, 1.0));

    highp vec2 inPos = sign(vertex.xy);
    vUV0 = vec2(inPos.x, -inPos.y) * 0.5 + 0.5;
    vRay = normal.xyz;
}
