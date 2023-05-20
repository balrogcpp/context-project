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
#define SMAA_REPROJECTION 0

//Can be sRGB
SAMPLER2D(rt_input, 0);
SAMPLER2D(blendTex, 1);
#if SMAA_REPROJECTION
SAMPLER2D(velocityTex, 2);
#endif
#include "smaa.glsl"


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(highp vec2 vUV0, TEXCOORD0)
IN(mediump vec4 offset, TEXCOORD1)

MAIN_DECLARATION
{
#if SMAA_REPROJECTION
	FragColor = SafeHDR(SMAANeighborhoodBlendingPS( vUV0, offset,
											   rt_input, blendTex, velocityTex ));
#else
	FragColor = SafeHDR(SMAANeighborhoodBlendingPS( vUV0, offset,
											   rt_input, blendTex ));
#endif
}
