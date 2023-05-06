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

#include "SMAA.hlsl"


in vec4 vertex;
in vec2 uv0;
uniform mat4 worldViewProj;
out vec2 uv0;
out vec2 pixcoord0;
out vec4 offset[3];


void main()
{
	gl_Position = worldViewProj * vertex;
	outVs.uv0 = uv0.xy;
	SMAABlendingWeightCalculationVS( uv0.xy, outVs.pixcoord0, outVs.offset );
}
