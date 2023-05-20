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
#include "smaa.glsl"


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(highp vec2 vUV0, TEXCOORD0)
IN(mediump vec2 pixcoord0, TEXCOORD1)
IN(mediump mat4 offset, TEXCOORD2)

MAIN_DECLARATION
{
	FragColor = SMAABlendingWeightCalculationPS(vUV0, pixcoord0, offset,
													edgeTex, areaTex, searchTex,
													vec4(0.0, 0.0, 0.0, 0.0));
}
