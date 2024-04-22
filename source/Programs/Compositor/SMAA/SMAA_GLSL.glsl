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

vec4 toSRGB(const vec4 col)
{
#ifndef FORCE_TONEMAP
	return vec4(LINEARtoSRGB(col.rgb), col.a);
#else
	return col;
#endif
}

vec4 fromSRGB(const vec4 srgb)
{
#ifndef FORCE_TONEMAP
	return vec4(SRGBtoLINEAR(srgb.rgb), srgb.a);
#else
	return srgb;
#endif
}

#endif // SMAA_GLSL
