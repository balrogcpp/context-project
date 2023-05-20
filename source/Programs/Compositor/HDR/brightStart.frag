// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"
#include "filters_RGB16.glsl"
SAMPLER2D(RT, 0);
SAMPLER2D(Lum, 1);
uniform mediump vec2 TexelSize0;
uniform mediump vec2 BrightThreshold;


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(highp vec2 vUV0, TEXCOORD0)

MAIN_DECLARATION
{
    mediump vec3 color = Downscale13(RT, vUV0, TexelSize0);
    mediump float lum = texture2D(Lum, vec2(0.0, 0.0)).r;

    color *= lum;

	mediump vec3 w = clamp((color - BrightThreshold.xxx) * BrightThreshold.yyy, 0.0, 1.0);
	color *= w * w * (3.0 - 2.0 * w);
    
    FragColor = vec4(color, 1.0);
}
