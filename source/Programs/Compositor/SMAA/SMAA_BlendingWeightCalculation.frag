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

#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1

uniform sampler2D edgeTex;
uniform sampler2D areaTex;
uniform sampler2D searchTex;
uniform vec4 ViewportSize;

#include "smaa.glsl"

in highp vec2 vUV0;
in vec2 pixcoord0;
in mat4 offset;
void main()
{
	FragColor = SMAABlendingWeightCalculationPS(vUV0, pixcoord0, offset, edgeTex, areaTex, searchTex, vec4(0.0, 0.0, 0.0, 0.0));
}
