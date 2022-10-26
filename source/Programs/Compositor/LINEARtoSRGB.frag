/// created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#include "srgb.glsl"

#ifndef MAX_SAMPLES
#define MAX_SAMPLES 4
#endif


in vec2 vUV0;
uniform sampler2D uSceneSampler;
uniform float uExposure;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  FragColor.rgb = LINEARtoSRGB(texture2D(uSceneSampler, vUV0).rgb, uExposure);
}
