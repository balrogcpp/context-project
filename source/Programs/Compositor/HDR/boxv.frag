// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif

#include "header.glsl"
#include "srgb.glsl"


varying mediump vec2 vUV0;
uniform sampler2D RT;
uniform mediump vec2 TexelSize0;

#define NUM_SAMPLES 11


void main()
{
	mediump vec2 uv = vUV0.xy;

	uv.x -= TexelSize0.x * (float(NUM_SAMPLES - 1) * 0.5);
	mediump vec3 sum = texture(RT, uv).rgb;

	for(mediump int i = 1; i < NUM_SAMPLES; ++i)
	{
		uv.x += TexelSize0.x;
		sum += texture2D(RT, uv).rgb;
	}

	FragColor = vec4(sum / float(NUM_SAMPLES), 1.0);
}
