#ifndef SMAA_GLSL
#define SMAA_GLSL

#ifndef SMAA_RT_METRICS
	#define SMAA_RT_METRICS ViewportSize.zwxy
#endif

#if !SMAA_INITIALIZED
	#define SMAA_PRESET_MEDIUM 1
	#define SMAA_GLSL_3 1
#endif


mediump vec4 toSRGB(mediump vec4 x )
{
	return LINEARtoSRGB(x);
}

mediump vec4 fromSRGB(mediump vec4 x )
{
	return SRGBtoLINEAR(x);
}

#endif // SMAA_GLSL
