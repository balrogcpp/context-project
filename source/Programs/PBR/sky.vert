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


attribute highp vec4 vertex;
attribute mediump vec4 uv0;

varying highp vec3 vPosition;
varying mediump vec3 vUV0; // direction vector representing a 3D texture coordinate

uniform highp mat4 WorldViewProj;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    vUV0 = uv0.xyz;
    vPosition = vertex.xyz;
    gl_Position = mul(WorldViewProj, vertex);
}
