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
in vec4 offset;
uniform sampler2D rt_input; //Can be sRGB
uniform sampler2D blendTex;
#if SMAA_REPROJECTION
	uniform sampler2D velocityTex;
#endif


void main()
{
#if SMAA_REPROJECTION
	FragColor = SMAANeighborhoodBlendingPS( inPs.uv0, inPs.offset,
											   rt_input, blendTex, velocityTex );
#else
	FragColor = SMAANeighborhoodBlendingPS( inPs.uv0, inPs.offset,
											   rt_input, blendTex );
#endif
}
