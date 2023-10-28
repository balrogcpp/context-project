#ifndef SMAA_GLSL
#define SMAA_GLSL

#ifndef SMAA_RT_METRICS
	#define SMAA_RT_METRICS ViewportSize.zwxy
#endif

#if !SMAA_INITIALIZED
	#define SMAA_PRESET_MEDIUM 1
	#define SMAA_GLSL_3 1
#endif

#include "srgb.glsl"

vec4 toSRGB(vec4 linear)
{
	return vec4(LINEARtoSRGB(linear.rgb), linear.a);
}

vec4 fromSRGB(vec4 srgb)
{
	return vec4(SRGBtoLINEAR(srgb.rgb), srgb.a);
}

#endif // SMAA_GLSL
