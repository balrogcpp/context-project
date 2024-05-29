// created by Andrey Vasiliev

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
uniform vec4 ViewportSize;

#include "smaa.glsl"

in highp vec2 vUV0;
in vec4 offset;
void main()
{
#if SMAA_REPROJECTION
	vec3 color = SMAANeighborhoodBlendingPS(vUV0, offset, rt_input, blendTex, velocityTex).rgb;
#else
	vec3 color = SMAANeighborhoodBlendingPS(vUV0, offset, rt_input, blendTex).rgb;
#endif

	FragColor.rgb = SafeHDR(color);
}
