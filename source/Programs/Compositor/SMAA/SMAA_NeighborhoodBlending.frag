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
uniform sampler2D rt_input;
uniform sampler2D blendTex;
#if SMAA_REPROJECTION
uniform sampler2D velocityTex;
#endif

uniform mediump vec4 ViewportSize;

#include "smaa.glsl"

in mediump vec2 vUV0;
in mediump vec4 offset;
void main()
{
#if SMAA_REPROJECTION
	FragColor = SafeHDR(SMAANeighborhoodBlendingPS( vUV0, offset,
											   rt_input, blendTex, velocityTex ));
#else
	FragColor = SafeHDR(SMAANeighborhoodBlendingPS( vUV0, offset,
											   rt_input, blendTex ));
#endif
}
