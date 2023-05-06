// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "SMAA_GLSL.glsl"
#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0

#include "SMAA.glsl"


attribute highp vec4 vertex;
attribute highp vec2 uv0;
uniform highp mat4 worldViewProj;
varying mediump vec2 vUV0;
varying mediump vec4 offset;


void main()
{
	gl_Position = worldViewProj * vertex;
	vUV0 = uv0.xy;
	SMAANeighborhoodBlendingVS( uv0.xy, offset );
}
