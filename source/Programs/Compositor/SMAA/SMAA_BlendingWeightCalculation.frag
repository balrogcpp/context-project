// created by Andrey Vasiliev

#include "header.glsl"
#include "SMAA_GLSL.glsl"

#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1

uniform sampler2D edgeTex;
uniform sampler2D areaTex;
uniform sampler2D searchTex;
uniform vec4 ViewportSize;

#include "smaa.glsl"

in vec2 vUV0;
in vec2 pixcoord0;
in mat4 offset;
void main()
{
	FragColor = SMAABlendingWeightCalculationPS(vUV0, pixcoord0, offset, edgeTex, areaTex, searchTex, vec4(0.0, 0.0, 0.0, 0.0));
}
