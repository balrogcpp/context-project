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
in vec4 offset[3];
uniform sampler2D rt_input;  //Must not be sRGB
#if SMAA_PREDICATION
	uniform sampler2D depthTex;
#endif


void main()
{
#if !SMAA_EDGE_DETECTION_MODE || SMAA_EDGE_DETECTION_MODE == 2
	#if SMAA_PREDICATION
		FragColor = vec4(SMAAColorEdgeDetectionPS( inPs.uv0, inPs.offset, rt_input, depthTex ), 0.0, 1.0);
	#else
		FragColor = vec4(SMAAColorEdgeDetectionPS( inPs.uv0, inPs.offset, rt_input ), 0.0, 1.0);
	#endif
#else
	#if SMAA_PREDICATION
		FragColor = vec4(SMAALumaEdgeDetectionPS( inPs.uv0, inPs.offset, rt_input, depthTex ), 0.0, 1.0);
	#else
		FragColor = vec4(SMAALumaEdgeDetectionPS( inPs.uv0, inPs.offset, rt_input ), 0.0, 1.0);
	#endif
#endif
}
