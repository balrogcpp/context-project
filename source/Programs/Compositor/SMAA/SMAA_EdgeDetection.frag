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
#if !SMAA_INITIALIZED
	#define SMAA_EDGE_DETECTION_MODE 1
	#define SMAA_PREDICATION 0
#endif

//Must not be sRGB
SAMPLER2D(rt_input, 0);
#if SMAA_PREDICATION
SAMPLER2D(depthTex, 1);
#endif
#include "smaa.glsl"

MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)
IN(mediump mat4 offset, TEXCOORD1)
MAIN_DECLARATION
{
#if !SMAA_EDGE_DETECTION_MODE || SMAA_EDGE_DETECTION_MODE == 2
	#if SMAA_PREDICATION
		FragColor = vec4(SMAAColorEdgeDetectionPS( vUV0, offset, rt_input, depthTex ), 0.0, 1.0);
	#else
		FragColor = vec4(SMAAColorEdgeDetectionPS( vUV0, offset, rt_input ), 0.0, 1.0);
	#endif
#else
	#if SMAA_PREDICATION
		FragColor = vec4(SMAALumaEdgeDetectionPS( vUV0, offset, rt_input, depthTex ), 0.0, 1.0);
	#else
		FragColor = vec4(SMAALumaEdgeDetectionPS( vUV0, offset, rt_input ), 0.0, 1.0);
	#endif
#endif
}
