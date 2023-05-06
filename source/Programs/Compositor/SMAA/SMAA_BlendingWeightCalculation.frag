#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "SMAA_GLSL.glsl"
#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1

#include "SMAA.hlsl"


in vec2 uv0;
in vec2 pixcoord0;
in vec4 offset[3];
uniform sampler2D edgeTex;
uniform sampler2D areaTex;
uniform sampler2D searchTex;


void main()
{
	FragColor = SMAABlendingWeightCalculationPS( inPs.uv0, inPs.pixcoord0, inPs.offset,
													edgeTex, areaTex, searchTex,
													vec4( 0, 0, 0, 0 ) );
}
