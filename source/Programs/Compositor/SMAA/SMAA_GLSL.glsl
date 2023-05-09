#ifndef SMAA_RT_METRICS
	#define SMAA_RT_METRICS ViewportSize.zwxy
#endif

#if !SMAA_INITIALIZED
	//Leave compatible defaults in case this file gets compiled
	//before calling SmaaUtils::initialize from C++
	#define SMAA_PRESET_ULTRA 1
	#define SMAA_GLSL_3 1
#endif


vec4 toSRGB( vec4 x )
{
	//return vec4(LINEARtoSRGB(x.rgb), x.a);
	return x;
}

vec4 fromSRGB( vec4 x )
{
	//return vec4(SRGBtoLINEAR(x.rgb), x.a);
	return x;
}
