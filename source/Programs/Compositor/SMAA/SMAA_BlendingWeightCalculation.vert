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

uniform mediump mat4 worldViewProj;
#include "smaa.glsl"

//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(highp vec4 vertex, POSITION)
IN(highp vec4 uv0, TEXCOORD0)
OUT(mediump vec2 vUV0, TEXCOORD0)
OUT(mediump vec2 pixcoord0, TEXCOORD1)
OUT(mediump mat4 offset, TEXCOORD2)

MAIN_DECLARATION
{
	gl_Position = worldViewProj * vertex;
	vUV0 = uv0.xy;
	SMAABlendingWeightCalculationVS( uv0.xy, pixcoord0, offset );
}
