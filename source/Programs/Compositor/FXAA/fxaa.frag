// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "filters.glsl"


in vec2 vUV0;
uniform sampler2D uSampler;
uniform vec2 TexelSize;
uniform float uFXAAStrength;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  FragColor.rgb = FXAA(uSampler, vUV0, TexelSize, uFXAAStrength);
}
