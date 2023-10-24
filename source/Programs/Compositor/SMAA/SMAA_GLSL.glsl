#ifndef SMAA_GLSL
#define SMAA_GLSL

#ifndef SMAA_RT_METRICS
	#define SMAA_RT_METRICS ViewportSize.zwxy
#endif

#if !SMAA_INITIALIZED
	#define SMAA_PRESET_MEDIUM 1
	#define SMAA_GLSL_3 1
#endif


float toSRGB(float x)
{
	return (x < 0.0031308 ? x * 12.92 : 1.055 * pow(x, 0.41666 ) - 0.055);
}

float fromSRGB(float x)
{
	return (x <= 0.040449907) ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}

vec4 toSRGB(vec4 x)
{
	return vec4(toSRGB(x.x), toSRGB(x.y), toSRGB(x.z), x.w);
}

vec4 fromSRGB(vec4 x)
{
	return vec4(fromSRGB(x.x ), fromSRGB(x.y), fromSRGB(x.z), x.w);
}

#endif // SMAA_GLSL
