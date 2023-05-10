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
#include "SMAA_GLSL.glsl"
#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0


attribute mediump vec4 vertex;
attribute mediump vec2 uv0;
uniform mediump mat4 worldViewProj;
varying mediump vec2 vUV0;
varying mediump vec4 offset;
uniform vec4 ViewportSize;
#include "smaa.glsl"


void main()
{
	gl_Position = worldViewProj * vertex;
	vUV0 = uv0.xy;
	SMAANeighborhoodBlendingVS( uv0.xy, offset );
}