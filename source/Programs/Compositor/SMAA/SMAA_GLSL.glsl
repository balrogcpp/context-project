#ifndef SMAA_GLSL
#define SMAA_GLSL

#ifndef SMAA_RT_METRICS
	#define SMAA_RT_METRICS ViewportSize.zwxy
#endif

#if !SMAA_INITIALIZED
	#define SMAA_PRESET_MEDIUM 1
	#define SMAA_GLSL_3 1
#endif

vec4 toSRGB(const vec4 col)
{
	//return vec4(sqrt(col.xyz), col.a);
	return col;
}

vec4 fromSRGB(const vec4 srgb)
{
	//return vec4(srgb.rgb * srgb.rgb, srgb.a);
	return srgb;
}

#endif // SMAA_GLSL
